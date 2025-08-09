#pragma once
#include "requestor.hpp"

namespace MyRpc{
    namespace Client
    {
        class RpcCaller{
            public:
                using ptr = std::shared_ptr<RpcCaller>;
                using JsonCallBack = std::function<void(const Json::Value&)>;
                // Requestor::ptr _requestor;
                //同步调用
                bool call(const ConnectionBase::ptr& conn, const std::string& method, 
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
                        return false;
                    }
                    if(rpc_resp->rcode() != Rcode::RCODE_OK){
                        ELOG("%s: %s",rpc_resp->GetId().c_str(),RcodeDesc[rpc_resp->rcode()].c_str());
                        return false;
                    }
                    result = rpc_resp->result();
                    return true;
                }
                //异步调用
                bool call(const ConnectionBase::ptr& conn, const std::string& method, 
                    const Json::Value& parameters,std::future<Json::Value>& result){
                    RpcRequest::ptr req = std::dynamic_pointer_cast<RpcRequest>(MessageFactory::create(Mtype::REQ_RPC));
                    req->SetId(Uuid::uuid());
                    req->SetType(Mtype::REQ_RPC);
                    req->setMethod(method);
                    req->setParameters(parameters);
                    //send接收Message::Base,未来会把response设置进传入的future<Message::ptr>中
                    //用户传入的是Json::Value,采用回调处理，在回调函数中对promise设置数据
                    // _requestor->send(conn,req,result);
                    std::shared_ptr<std::promise<Json::Value>> promise_value;
                    result = promise_value->get_future();
                    Requestor::ResponseCallBack func = std::bind(&RpcCaller::CallOnAsync,this,promise_value,
                        std::placeholders::_1);
                    _requestor->send(conn,req,func);
                    return true;
                }
                //回调
                bool call(const ConnectionBase::ptr& conn, const std::string& method,
                    const Json::Value& parameters,JsonCallBack& call){
                    RpcRequest::ptr req = std::dynamic_pointer_cast<RpcRequest>(MessageFactory::create(Mtype::REQ_RPC));
                    req->SetId(Uuid::uuid());
                    req->SetType(Mtype::REQ_RPC);
                    req->setMethod(method);
                    req->setParameters(parameters);
                    //用户传入对Json::Value的回调，requestor需要传入对Message::ptr的回调,通过CallOnMessage解决
                    Requestor::ResponseCallBack call_back = std::bind(&RpcCaller::CallOnMessage,this,call,
                        std::placeholders::_1);
                    _requestor->send(conn,req,call_back);
                    return true;
                }
                RpcCaller(const Requestor::ptr& requestor):_requestor(requestor){}
            private:
                //处理结果
                void CallOnMessage(JsonCallBack& jsonCall,MessageBase::ptr& msg){
                    auto resp = std::dynamic_pointer_cast<RpcResponse>(msg);
                    if(!resp){
                        ELOG("响应类型转换失败!");  
                    }
                    if(resp->rcode() != Rcode::RCODE_OK){
                        ELOG("%s: %s",resp->GetId().c_str(),RcodeDesc[resp->rcode()].c_str());
                        return;
                    }
                    jsonCall(resp->result());
                }
                //设置结果
                void CallOnAsync(std::shared_ptr<std::promise<Json::Value>> rsp,MessageBase::ptr& msg)
                {
                    auto resp = std::dynamic_pointer_cast<RpcResponse>(msg);
                    if(!resp){
                        ELOG("响应类型转换失败!");  
                    }
                    if(resp->rcode() != Rcode::RCODE_OK){
                        ELOG("%s: %s",resp->GetId().c_str(),RcodeDesc[resp->rcode()].c_str());
                        return;
                    }
                    rsp->set_value(resp->result());
                }
                Requestor::ptr _requestor;
        };
    } // namespace Client
    
}