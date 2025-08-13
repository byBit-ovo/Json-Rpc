#include "../network/dispatcher.hpp"
#include "rpc_caller.hpp"
#include "service_router.hpp"

namespace MyRpc{
    namespace Client{
        //服务注册客户端
        class RegisterClient{
            public:
                using ptr = std::shared_ptr<RegisterClient>;
                //构造函数中连接注册中心...
                RegisterClient(const std::string &ip, int port):
                _requestor(std::make_shared<Requestor>()),
                _provider(std::make_shared<Provider>(_requestor)),
                _dispatcher(std::make_shared<Dispatcher>()){
                    auto rpc_rsp_call = std::bind(&MyRpc::Client::Requestor::onResponse,_requestor.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    //向dispatcher注册服务注册响应的回调 
                    _dispatcher->registerHandler<MyRpc::MessageBase>(MyRpc::Mtype::RSP_SERVICE,rpc_rsp_call);
                    auto call_back = std::bind(&MyRpc::Dispatcher::messageCallBack,_dispatcher.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    MyRpc::ClientBase::ptr client = MyRpc::ClientFactory::create(ip,port);
                    _client->SetMessageCallBack(call_back);
                }
                void connect(){
                    _client->connect();
                }
                //向注册中心注册服务
                bool registerMethod(const std::string &method,const Address& host){
                    if(_client->connected() == false){
                        ELOG("连接尚未建立!");
                        return false;
                    }
                    return _provider->registerMethod(_client->connection(),method,host);
                }
            private:
                Requestor::ptr _requestor;      // communication core
                Provider::ptr _provider;        //通过_requestor发送服务注册请求 
                Dispatcher::ptr _dispatcher;    //通过_requestor中的onResponse接收响应
                ClientBase::ptr _client;        //has a connection
        };
        //服务发现客户端
        class DiscoverClient{
            public:
                using ptr = std::shared_ptr<DiscoverClient>;
                //构造函数中连接注册中心...
                DiscoverClient(const std::string &ip, int port):
                _requestor(std::make_shared<Requestor>()),
                _discoverer(std::make_shared<Discoverer>(_requestor)),
                _dispatcher(std::make_shared<Dispatcher>()){
                    auto rpc_rsp_call = std::bind(&MyRpc::Client::Requestor::onResponse,_requestor.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    //向dispatchetr注册发现者收到注册响应的回调
                    _dispatcher->registerHandler<MyRpc::MessageBase>(MyRpc::Mtype::RSP_SERVICE,rpc_rsp_call);
                    auto line_req = std::bind(&Discoverer::onServiceRequest,_discoverer.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    //向dispatcher注册发现者收到服务上下线请求的回调
                    _dispatcher->registerHandler<ServiceRequest>(MyRpc::Mtype::REQ_SERVICE,line_req);
                    auto call_back = std::bind(&MyRpc::Dispatcher::messageCallBack,_dispatcher.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    MyRpc::ClientBase::ptr client = MyRpc::ClientFactory::create(ip,port);
                    _client->SetMessageCallBack(call_back);
                }
                void connect(){
                    _client->connect();
                }
                //向注册中心发现服务
                bool serviceDiscover(std::string &method, Address &host){
                    if(_client->connected() == false){
                        ELOG("连接尚未建立!");
                        return false;
                    }
                    return _discoverer->serviceDiscover(_client->connection(),method,host);
                }
            private:
                Requestor::ptr _requestor;       //communication core
                Discoverer::ptr _discoverer;     //通过_requestor发送服务发现请求
                Dispatcher::ptr _dispatcher;     //通过_requestor中的onResponse接收请求
                ClientBase::ptr _client;         //has a connction

        };

        //Rpc调用客户端，同时可以进行服务发现
        class RpcClient{
            public:
                using ptr = std::shared_ptr<RpcClient>;
                //构造函数中连接注册中心...,选择是否启用服务发现功能
                //同时决定该ip是服务提供者地址还是服务发现的地址
                RpcClient(bool enable_discover,const std::string &ip, int port){}
                //向注册中心发现服务
                bool serviceDiscover(std::string &method, Address &host);
                bool call(const std::string& method, const Json::Value& parameters,Json::Value& result){
                    
                }
                bool call(const std::string& method, const Json::Value& parameters,std::future<Json::Value>& result){

                }
                bool call(const std::string& method,const Json::Value& parameters,RpcCaller::JsonCallBack& call){
                    
                }
            private:
                bool _enable_discover;
                DiscoverClient::ptr _discover_client;
                RpcCaller::ptr _caller;
                Requestor::ptr _requestor;
                Dispatcher::ptr _dispatcher;
                ClientBase::ptr _rpc_client;

        };
    }
}