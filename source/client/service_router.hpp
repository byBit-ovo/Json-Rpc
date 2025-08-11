#pragma once
#include "requestor.hpp"
//客户端的服务发现方和服务注册方

namespace MyRpc{
    namespace Client{
        class Provider{
            public:
                Provider(const Requestor::ptr &requestor):_requestor(requestor){}
                using ptr = std::shared_ptr<Provider>;
                bool registerMethod(const ConnectionBase::ptr &conn,const std::string &method,const Address& host)
                {
                    auto msg = MessageFactory::create<ServiceRequest>();
                    msg->SetId(Uuid::uuid());
                    msg->SetType(Mtype::REQ_SERVICE);
                    msg->setServiceOpType(ServiceOptype::SERVICE_REGISTRY);
                    msg->setMethod(method);
                    msg->setHost(host);
                    MessageBase::ptr res;
                    bool ret = _requestor->send(conn,msg,res);
                    if(ret == false){
                        ELOG("%s 服务注册失败!",method.c_str());
                        return false;
                    }
                    auto rsp = std::dynamic_pointer_cast<ServiceResponse>(res);
                    if(rsp.get() == nullptr){
                        ELOG("类型转换失败!");
                        return false;
                    }
                    if(rsp->rcode() != Rcode::RCODE_OK){
                        ELOG("服务注册失败: %s", ErrReason(rsp->rcode()));
                        return false;
                    }
                    return true;

                }
            private:
                Requestor::ptr _requestor;
        };
    }
}