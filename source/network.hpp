#pragma once
#include "muduo/net/Buffer.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "abstract.hpp"
#include "message.hpp"
#include <arpa/inet.h>
#include <unordered_map>
#include <mutex>
namespace MyRpc
{
    class MuduoBuffer : public BufferBase
    {
    public:
        using ptr = std::shared_ptr<MuduoBuffer>;
        RpcBuffer(muduo::net::Buffer *buf) : _buf(buf) {}
        virtual size_t readableSize()
        {
            return _buf->readableBytes();
        }
        virtual int peekInt()
        {
            return _buf->peekInt32();
        }
        virtual void retrieveInt32()
        {
            _buf->retrieveInt32();
        }
        virtual int readInt32()
        {
            return _buf->readInt32();
        }
        virtual std::string readAsString(size_t len)
        {
            return _buf->retrieveAsString(len);
        }

    private:
        muduo::net::Buffer *_buf;
    };

    class BufferFactory
    {
    public:
        template <class... Args>
        static BufferBase::ptr create(Args &&...args)
        {
            return std::make_shared<MuduoBuffer>(std::forward<Args>(args)...);
        }
    };

    class RpcProtocol : public ProtocolBase
    {
    public:
        using ptr = std::shared_ptr<RpcProtocol>;
        // 判断缓冲区是否存在一条完整的消息
        // 8 message(8 bytes)
        //|--headLen--|--Mtype--|--idLen-- |------id--------|--body--|
        //|--4bytes---|--4bytes-|--4bytes--|--idlen bytes---|--body--|
        //|--4bytes---|-----------------headLen bytes----------------|
        virtual bool canProceed(const BufferBase::ptr &buffer) override
        {
            int len = buffer->peekInt();
            return buffer->readableSize() >= len + _headLen;
        }
        virtual bool recieveAmessage(const BufferBase::ptr &buffer, MessageBase::ptr &msg) override
        {
            // 在调用此函数之前需要先判断canProceed
            int len = buffer->readInt32();
            int mtype = buffer->readInt32();
            int idlen = buffer->readInt32();
            std::string id = buffer->readAsString(idlen);
            std::string body = buffer->readAsString(len - _typeLen - _idLen - idlen);
            msg = MessageFactory::create(static_cast<Mtype>(mtype));
            if (msg.get() == nullptr)
            {
                ELOG("消息类型错误,解析失败!");
                return false;
            }
            msg->deserialize(body);
            msg->SetId(id);
            msg->SetType(static_cast<Mtype>(mtype));
            return true;
        }
        virtual std::string serialize(const MessageBase::ptr &msg)
        {
            //|--headLen--|--Mtype--|--idLen-- |------id--------|--body--|
            std::string body = msg->serialize();
            std::string id = msg->GetId();
            int mtype = htonl(static_cast<int>(msg->GetType()));
            int len = htonl(_typeLen + _idLen + id.size() + body.size());
            std::stringstream ss;
            ss << len << mtype << htonl(id.size()) << id << body;
            return ss.str();
        }

    private:
        static const size_t _headLen;
        static const size_t _typeLen;
        static const size_t _idLen;
    };
    const size_t RpcProtocol::_headLen = sizeof(int);
    const size_t RpcProtocol::_typeLen = sizeof(int);
    const size_t RpcProtocol::_idLen = sizeof(int);

    class ProtocolFactory
    {
    public:
        template <class... Args>
        static ProtocolBase::ptr create(Args... args)
        {
            return std::make_shared<RpcProtocol>(std::forward<Args>(args)...);
        }
    };

    class RpcConnection : public ConnectionBase
    {
    public:
        using ptr = std::shared_ptr<RpcConnection>;
        RpcConnection(const ProtocolBase::ptr &protocol, const muduo::net::TcpConnectionPtr &conn) :
         _protocol(protocol), _conn(conn) {}

        virtual void send(const MessageBase::ptr &msg) override
        {
            std::string message = _protocol->serialize(msg);
            _conn->send(message);
        }
        virtual void shutDown() override
        {
            _conn->shutdown();
        }
        virtual bool isConnected() override
        {
            return _conn->connected();
        }

    private:
        ProtocolBase::ptr _protocol;
        muduo::net::TcpConnectionPtr _conn;
    };
    class ConnectionFactory
    {
    public:
        template <class... Args>
        static ConnectionBase::ptr create(Args... args)
        {
            return std::make_shared<RpcConnection>(std::forward<Args>(args)...);
        }
    };

    class MuduoServer : public ServerBase
    {
    public:
        using ptr = std::shared_ptr<MuduoServer>;
        MuduoServer(int port) : _tcpsvr(&_baseloop, muduo::net::InetAddress("0.0.0.0", port),
        "MuduoServer", muduo::net::TcpServer::kReusePort),_protocol(ProtocolFactory::create())
        {
            _tcpsvr.setConnectionCallback(std::bind(&MuduoServer::ConnectionCallBack, this, std::placeholders::_1));
            _tcpsvr.setMessageCallback(std::bind(&MuduoServer::MessageCallBack, this, 
            std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
        }

        virtual void start()
        {
            _tcpsvr.start();
            _baseloop.loop();
        }

    private:
        muduo::net::EventLoop _baseloop;
        muduo::net::TcpServer _tcpsvr;
        ProtocolBase::ptr _protocol;
        std::unordered_map<muduo::net::TcpConnectionPtr,ConnectionBase::ptr> _connections;
        std::mutex _lock;
        static const size_t msgMaxLen;
        void ConnectionCallBack(const muduo::net::TcpConnectionPtr &conn)
        {
            if (conn->connected())
            {
                ILOG("新链接建立成功!");
                {
                    std::lock_guard<std::mutex> guard(_lock);
                    _connections.insert(std::make_pair(conn, ConnectionFactory::create(_protocol, conn)));
                }
                if(_connection_call_back){
                    _connection_call_back(_connections[conn]);
                }
            }
            else
            {
                ConnectionBase::ptr rpcConn;
                ILOG("连接断开!");
                {
                    std::lock_guard<std::mutex> guard(_lock);
                    auto iter  = _connections.find(conn);
                    if(iter == _connections.end()){
                        return;
                    }
                    rpcConn = iter->second;
                    _connections.erase(conn);
                }
                if(_close_call_back){
                    _close_call_back(rpcConn);
                }
            }
        }
        void MessageCallBack(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf, muduo::Timestamp)
        {
            BufferBase::ptr buf_base = BufferFactory::create(buf);
            while(true)
            {
                if(_protocol->canProceed(buf_base) == false){
                    if(buf_base->readableSize() >= msgMaxLen){
                        conn->shutdown();
                        ELOG("缓冲区数据过长!");
                        return;
                    }
                    break;
                }
                MessageBase::ptr msg;
                if(_protocol->recieveAmessage(buf_base,msg) == false){
                    conn->shutdown();
                    ELOG("缓冲区数据解析错误!");
                    break;
                }
                ConnectionBase::ptr rpcConn;
                {
                    std::lock_guard<std::mutex> guard(_lock);
                    auto iter = _connections.find(conn);
                    if(iter == _connections.end()){
                        ELOG("连接关系错误,找不到对应关系!");
                        conn->shutdown();
                        return;
                    }
                    rpcConn = iter->second;
                }
                if(_message_call_back){
                    _message_call_back(rpcConn, msg);
                }
            }
        }
    };

    const size_t MuduoServer::msgMaxLen = (1 << 16);
}