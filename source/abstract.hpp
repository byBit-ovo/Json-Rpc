#include <functional>
#include <memory>
#include "Util.hpp"
namespace MyRpc
{
    //length - mtype - idlength - id - body
    class MessageBase
    {
        private:
            Mtype _type;
            std::string _id;
        public:
            using ptr = std::shared_ptr<BufferBase>;
            ~MessageBase(){}
            virtual void SetId(const std::string& id){
                _id = id;
            }
            virtual void SetType(Mtype type){
                _type = type;
            }
            virtual Mtype GetType(){return _type;}
            virtual std::string GetId(){return _id;}
            virtual std::string serialize()=0; 
            virtual bool deserialize(const std::string& msg)= 0 ;
            virtual bool check() = 0; 
    };

    class BufferBase
    {
        public:
            using ptr = std::shared_ptr<BufferBase>;
            virtual size_t readableSize() = 0;
            virtual int peekInt() = 0;
            virtual void retrieveInt32() = 0;
            virtual int readInt32() = 0;
            virtual std::string readAsString(size_t len) = 0;
    };

    class ProtocolBase
    {
        public:
            using ptr = std::shared_ptr<ProtocolBase>;
            virtual bool canProcess(const BufferBase::ptr& buffer) = 0;
            virtual bool Onmessage(const BufferBase::ptr& buffer, MessageBase::ptr& msg) = 0;
            virtual std::string serialize(const MessageBase::ptr& buffer) = 0;
    };

    class ConnectionBase
    {
        public:
            using ptr = std::shared_ptr<ConnectionBase>;
            virtual void send(const MessageBase::ptr& msg) = 0;
            virtual void shutDown() = 0;
            virtual bool isConnected() = 0;
    };

    using ConnectionCallBack = std::function<void(const ConnectionBase::ptr&)>;
    using CloseCallBack = std::function<void(const ConnectionBase::ptr&)>;
    using MessageCallBack = std::function<void(const ConnectionBase::ptr&, BufferBase::ptr&)>;
    using ptr = std::shared_ptr<ServerBase>;
    class ServerBase
    {
        public:
            virtual void SetConnectionCallBack(const ConnectionCallBack& func){
                _connection_call_back = func;
            }
            virtual void SetCloseCallBack(const CloseCallBack& func){
                _close_call_back = func;
            }
            virtual void SetConnectionCallBack(const MessageCallBack& func){
                _message_call_back = func;
            }
            virtual void start() = 0;
        private:
            ConnectionCallBack _connection_call_back;
            CloseCallBack _close_call_back;
            MessageCallBack _message_call_back;

    };

    class ClientBase
    {
       
        private:
            ConnectionCallBack _connection_call_back;
            CloseCallBack _close_call_back;
            MessageCallBack _message_call_back;
        public:
            virtual void SetConnectionCallBack(const ConnectionCallBack& func){
                _connection_call_back = func;
            }
            virtual void SetCloseCallBack(const CloseCallBack& func){
                _close_call_back = func;
            }
            virtual void SetConnectionCallBack(const MessageCallBack& func){
                _message_call_back = func;
            }
            virtual void connect() = 0;
            virtual void shutDown() = 0;
            virtual void send(const MessageBase::ptr& msg)=0;
            virtual bool connected() = 0;
            virtual ConnectionBase::ptr connection()=0; 
    }

}