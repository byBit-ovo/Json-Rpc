#pragma once
#include "../network/dispatcher.hpp"
#include "../client/rpc_client.hpp"
#include "rpc_router.hpp"
#include "service_manager.hpp"

namespace MyRpc{
    namespace Server{
        //管理服务的注册与发现请求
        class RegisterServer{
            public:
                using ptr = std::shared_ptr<RegisterServer>;
                RegisterServer(int port):
                _server(ServerFactory::create(port)),
                _dispatcher(std::make_shared<Dispatcher>()),
                _service_manager(std::make_shared<ServiceManager>()){
                    auto service_req = std::bind(&ServiceManager::onServiceRequest,_service_manager.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    //注册 '服务发现与服务注册' 的回调函数
                    _dispatcher->registerHandler<ServiceRequest>(Mtype::REQ_SERVICE,service_req);
                    auto message_call = std::bind(&Dispatcher::messageCallBack,_dispatcher.get(),
                    std::placeholders::_1,std::placeholders::_2);
                    //MuduoServer的总接口
                    auto close_call_back = std::bind(&RegisterServer::onClose,this,std::placeholders::_1);
                    _server->SetMessageCallBack(message_call);
                    _server->SetCloseCallBack(close_call_back);
                }
                void start(){
                    _server->start();
                }
            private:
                void onClose(const ConnectionBase::ptr &conn){
                    _service_manager->onShutDown(conn);
                }
                ServerBase::ptr _server;
                Dispatcher::ptr _dispatcher; 
                ServiceManager::ptr _service_manager;
        };
        //Rpc调用服务，同时可以选择是否包含服务注册功能
        //如果收到服务注册请求，帮客户端转发给服务注册中心
        class RpcServer{
            public:
                using ptr = std::shared_ptr<RpcServer>;
                RpcServer(int port):
                _server(ServerFactory::create(port)),
                _dispatcher(std::make_shared<Dispatcher>()),
                _router(std::make_shared<RpcRouter>()){
                    auto rpc_call = std::bind(&RpcServer::onRpcReq,this,std::placeholders::_1,std::placeholders::_2);
                    _server->SetMessageCallBack(rpc_call);
                }
                void registerService(const ServiceDesc::ptr& service){
                }
                void start(){
                    _server->start();
                }
            private:
                void onRpcReq(const ConnectionBase::ptr &conn,const MessageBase::ptr &msg){
                    auto rpc_req = std::dynamic_pointer_cast<RpcRequest>(msg);
                    _router->onRpcRequest(conn,rpc_req);
                }
                bool _enable_register;
                Client::RegisterClient::ptr _register_client;
                ServerBase::ptr _server;
                Dispatcher::ptr _dispatcher; 
                RpcRouter::ptr _router;
        };
    }
}
