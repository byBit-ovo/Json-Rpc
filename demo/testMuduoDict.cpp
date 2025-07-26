#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/Buffer.h"
#include "muduo/net/TcpConnection.h"
#include "muduo/net/InetAddress.h"
#include <iostream>
#include <string>
#include <unordered_map>


class DictServer
{
    private:
        muduo::net::EventLoop _baseloop;
        muduo::net::TcpServer _tcpsvr;
        std::unordered_map<std::string,std::string> _dict;
    public:
        void Start(){
            _tcpsvr.start();
            _baseloop.loop();
        }
        DictServer(int port):_tcpsvr(&_baseloop,muduo::net::InetAddress("0.0.0.0",port),"DictServer"){
            _dict["hello"] = "你好";
            _dict["sky"] = "天空";
            _dict["bird"] = "小鸟";
            _dict["river"] = "河流";
            _dict["street"] = "街道";
            _tcpsvr.setConnectionCallback(std::bind(&DictServer::ConnectionCallBack,this,std::placeholders::_1));
            _tcpsvr.setMessageCallback(std::bind(&DictServer::MessageCallBack,this,std::placeholders::_1,
            std::placeholders::_2,std::placeholders::_3));

        }
        void ConnectionCallBack(const muduo::net::TcpConnectionPtr& conn){
            if(conn->connected()){
                std::cout<<"新链接建立成功"<<std::endl;
            }
            else
            {
                std::cout<<"连接建立失败";
            }
        }
        void MessageCallBack(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer *buf,muduo::Timestamp)
        {
            std::string query = buf->retrieveAllAsString();
            auto it = _dict.find(query);
            std::string res;
            if(it != _dict.end()){
                res  = _dict[query];
            }
            else
            {
                res = "抱歉，未查询到此单词";
            }
            conn->send(res);
        }

};



int main(int argc,char **argv)
{
    if(argc != 2){
        return 1;
    }
    int port = std::stoi(argv[1]);
    DictServer svr(port);
    svr.Start();
    return 0;
}