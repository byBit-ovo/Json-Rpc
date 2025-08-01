#include "message.hpp"
void RpcMessageTest()
{
    MyRpc::RpcRequest::ptr rrq = std::dynamic_pointer_cast<MyRpc::RpcRequest>(MyRpc::MessageFactory::create(MyRpc::Mtype::REQ_RPC));
    rrq->SetId(Uuid::uuid());
    rrq->setMethod("Add");
    Json::Value params;
    params["num1"] = 1;
    params["num2"] = 4;
    rrq->setParameters(params);
    std::string msg = rrq->serialize();
    std::cout<<msg<<std::endl;
}
class Test
{
    public:
        Test(){
            ILOG("Test()");
        }
        ~Test(){
            ILOG("~Test()");
        }
};
int main()
{
    RpcMessageTest();
    return 0;
}