#include "../network/network.hpp"
#include "../network/message.hpp"
#include <future>
namespace MyRpc
{
    namespace Client
    {
        //为了明确客户端接受到的响应对应于哪一条请求，Requestor用于保存请求与响应之间的映射关系
        class Requestor
        {
            public:
                using ResponseCallBack = std::function<void(MessageBase::ptr)>;
                struct RequestDesc{
                    using ptr = std::shared_ptr<RequestDesc>;
                    MessageBase::ptr _req;
                    ReqType _type;
                    std::promise<MessageBase::ptr> response;
                    ResponseCallBack _call_back;
                };
                void send(const ConnectionBase::ptr& conn,)
            private:
                std::mutex _lock;
                std::unordered_map<std::string, RequestDesc::ptr> _requests_desc;
        };
    }
}