#include "../network/dispatcher.hpp"
#include "rpc_caller.hpp"
#include "service_router.hpp"

namespace MyRpc{
    namespace Client{
        class RegisterClient{
            public:
                using ptr = std::shared_ptr<RegisterClient>;
                //构造函数中连接注册中心...
                RegisterClient(const std::string &ip, int port){}
                //向注册中心注册服务
                bool registerMethod(const std::string &method,const Address& host){

                }
            private:
                Provider::ptr _provider;
                Requestor::ptr _requestor;
                Dispatcher::ptr _dispatcher;
                ClientBase::ptr _client;
        };

        class DiscoverClient{
            public:
                using ptr = std::shared_ptr<DiscoverClient>;
                //构造函数中连接注册中心...
                DiscoverClient(const std::string &ip, int port){}
                //向注册中心发现服务
                bool serviceDiscover(std::string &method, Address &host);
            private:
                Discoverer::ptr _discoverer;
                Requestor::ptr _requestor;
                Dispatcher::ptr _dispatcher;
                ClientBase::ptr _client;

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