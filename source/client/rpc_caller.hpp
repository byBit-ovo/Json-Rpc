#pragma once
#include "requestor.hpp"

namespace MyRpc{
    namespace Client
    {
        class RpcCaller{
            public:
                using ptr = std::shared_ptr<RpcCaller>();
                using CallBack = std::function<void(const Json::Value&)>;
                // Requestor::ptr _requestor;
                //同步调用
                void call(const ConnectionBase::ptr& conn, const std::string& method, 
                    const Json::Value& parameters,Json::Value& result){
                    RpcRequest::ptr req = std::dynamic_pointer_cast<RpcRequest>(MessageFactory::create(Mtype::REQ_RPC));
                    req->SetId(Uuid::uuid());
                    req->SetType(Mtype::REQ_RPC);
                    req->setMethod(method);
                    req->setParameters(parameters);
                    MessageBase::ptr response;
                    _requestor->send(conn,req,response);
                    RpcResponse::ptr rpc_resp = std::dynamic_pointer_cast<RpcResponse>(response);
                    if(!rpc_resp){
                        ELOG("响应类型转换失败!");
                    }
                    if(rpc_resp->rcode() != Rcode::RCODE_OK){
                        ELOG("%s: %s",rpc_resp->GetId(),RcodeDesc[rpc_resp->rcode()]);
                        return;
                    }
                    result = rpc_resp->result();
                }
                //异步调用
                void call(const ConnectionBase::ptr& conn, const std::string& method, 
                    const Json::Value& parameters,std::future<Json::Value>& result){
                    RpcRequest::ptr req = std::dynamic_pointer_cast<RpcRequest>(MessageFactory::create(Mtype::REQ_RPC));
                    req->SetId(Uuid::uuid());
                    req->SetType(Mtype::REQ_RPC);
                    req->setMethod(method);
                    req->setParameters(parameters);
                    _requestor->send(conn,req,result);
                }
                //回调
                void call(const ConnectionBase::ptr& conn, const std::string& method,
                    const Json::Value& parameters,CallBack& call){
                    RpcRequest::ptr req = std::dynamic_pointer_cast<RpcRequest>(MessageFactory::create(Mtype::REQ_RPC));
                    req->SetId(Uuid::uuid());
                    req->SetType(Mtype::REQ_RPC);
                    req->setMethod(method);
                    req->setParameters(parameters);

                }
                RpcCaller(const Requestor::ptr& requestor):_requestor(requestor){}
            private:
                Requestor::ptr _requestor;
        };
    } // namespace Client
    
}