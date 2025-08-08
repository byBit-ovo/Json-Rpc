#include "../network/network.hpp"
#include "../network/message.hpp"
#include "../network/dispatcher.hpp"
#include <future>
namespace MyRpc
{
    namespace Client
    {
        //多线程环境中，为了明确客户端接受到的响应对应于哪一条请求，Requestor用于保存请求与响应之间的映射关系
        class Requestor
        {
            public:
                using ResponseCallBack = std::function<void(MessageBase::ptr)>;
                //将发出的请求保管起来，等到拿到对应的响应，再返回给上层,否则无法确定收到的响应对应哪一条请求
                struct RequestDesc{
                    using ptr = std::shared_ptr<RequestDesc>;
                    ReqType _type; 
                    MessageBase::ptr _req;
                    //如果是异步请求，将对应的响应放入到promise
                    std::promise<MessageBase::ptr> _response;
                    //如果是同步请求，调用该函数
                    ResponseCallBack _call_back;
                };
                //该函数注册给dispatcher, 收到response时，,将response设置进对应的RequestDesc或回调
                void onResponse(const ConnectionBase::ptr &conn, MessageBase::ptr& msg){
                    auto desc = find(msg->GetId());
                    if(desc.get() == nullptr){
                        ELOG("收到的响应未找到请求描述!请求id: %s", msg->GetId().c_str());
                        return;
                    }
                    if(desc->_type == ReqType::REQ_ASYNC){
                        desc->_response.set_value(msg);
                    }
                    else if(desc->_type == ReqType::REQ_CALLBACK){
                        if(desc->_call_back)
                            desc->_call_back(msg);
                    }
                    else{
                        ELOG("收到了非Async,非回调请求对应的响应");
                    }
                }
                void send(const ConnectionBase::ptr& conn, MessageBase::ptr& msg,ResponseCallBack& call)
                {
                    RequestDesc::ptr desc = insertDesc(msg,ReqType::REQ_CALLBACK);
                    desc->_call_back = call;
                    
                }
                //异步
                //上层在发送请求时传入future,将其关联到ReqDesc中的promise,onResponse后，会将收到的响应设置进future,外部通过get获取响应
                void send(const ConnectionBase::ptr& conn, MessageBase::ptr& msg,std::future<MessageBase::ptr>& resp)
                {
                    RequestDesc::ptr desc = insertDesc(msg,ReqType::REQ_ASYNC);
                    resp = desc->_response.get_future();
                    conn->send(msg);
                }
                //同步获取响应
                void send(const ConnectionBase::ptr& conn, MessageBase::ptr& msg,MessageBase::ptr& resp){
                    std::future<MessageBase::ptr> resp_future;
                    send(conn,msg,resp_future);
                    //直接get达到同步的效果
                    resp = resp_future.get();
                }
            private:
                //添加请求描述
                RequestDesc::ptr insertDesc(const MessageBase::ptr& req, ReqType type,
                    const ResponseCallBack& call=ResponseCallBack()){
                    std::unique_lock<std::mutex> guard(_lock);
                    RequestDesc::ptr desc = std::make_shared<RequestDesc>();
                    desc->_type = type;
                    desc->_req = req;
                    if(type == ReqType::REQ_CALLBACK && call){
                        desc->_call_back = call;
                    }
                    _requests_desc.insert(std::make_pair(req->GetId(),desc));
                    return desc;
                }
                //移除请求描述
                void removeDesc(const std::string& id){
                    std::unique_lock<std::mutex> guard(_lock);
                    _requests_desc.erase(id);
                }
                //查找请求描述
                RequestDesc::ptr find(const std::string& id){
                    auto iter = _requests_desc.find(id);
                    if(iter==_requests_desc.end()){
                        return RequestDesc::ptr();
                    }
                    return iter->second;
                }
                std::mutex _lock;
                //                      uuid(): RequestDesc
                std::unordered_map<std::string, RequestDesc::ptr> _requests_desc;
        };
    }
}