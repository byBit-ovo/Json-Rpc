#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/Buffer.h>
#include <muduo/base/CountDownLatch.h>
#include <muduo/net/EventLoopThread.h>
#include <iostream>
#include <string>
#include <thread>
class DictClient
{
    private:
        muduo::net::EventLoopThread _thread;
        muduo::net::TcpConnectionPtr _conn;
        muduo::net::EventLoop *_baseloop;
        muduo::net::TcpClient _client;
        muduo::CountDownLatch _cdl;
        void ConnectionCallBack(const muduo::net::TcpConnectionPtr& conn){
            if(conn->connected()){
                //--count
                _cdl.countDown();
                _conn = conn;
                std::cout<<"连接服务器成功"<<std::endl;
            }
            else
            {
                std::cout<<"连接服务器失败"<<std::endl;
                _conn.reset();
            }
        }
        void MessageCallBack(const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer *buf,muduo::Timestamp)
        {
            std::string res = buf->retrieveAllAsString();
            std::cout<<res<<std::endl;
            std::cout<<std::this_thread::get_id()<<std::endl;
        }
    public:
        DictClient(const std::string ip,int port):
            _baseloop(_thread.startLoop()),
            _client(_baseloop,muduo::net::InetAddress(ip,port),"client"),_cdl(1){
            _client.setConnectionCallback(std::bind(&DictClient::ConnectionCallBack,this,std::placeholders::_1));
            _client.setMessageCallback(std::bind(&DictClient::MessageCallBack,this,std::placeholders::_1,
            std::placeholders::_2,std::placeholders::_3));
            _client.connect();
            //等待所有连接成功，为0时唤醒 
            _cdl.wait();
        }

        void send(const std::string& message)
        {
            if(_conn->disconnected()){
                return ;
            }
            _conn->send(message);
        }
};
int main(int argc,char **argv)
{
    if(argc != 2){
        return 1;
    }
    int port = std::stoi(argv[1]);
    DictClient client("127.0.0.1",port);
    std::string query;
    while(true)
    {
        std::cout<<std::this_thread::get_id()<<std::endl;
        std::cin>>query;
        client.send(query);
    }
    return 0;
}