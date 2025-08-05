#include "network.hpp"
#include "dispatcher.hpp"
void RpcMessageTest()
{
    // method 1:
    MyRpc::RpcRequest::ptr rrq = std::dynamic_pointer_cast<MyRpc::RpcRequest>(MyRpc::MessageFactory::create(MyRpc::Mtype::REQ_RPC));
    MyRpc::RpcRequest::ptr rrq4 = MyRpc::MessageFactory::create<MyRpc::RpcRequest>();
    rrq->SetId(Uuid::uuid());
    rrq->setMethod("Add");
    Json::Value params;
    params["num1"] = 1;
    params["num2"] = 4;
    rrq->setParameters(params);
    std::string msg = rrq->serialize();
    std::cout << msg << std::endl;
    MyRpc::RpcRequest::ptr rrq2 = std::dynamic_pointer_cast<MyRpc::RpcRequest>(MyRpc::MessageFactory::create(MyRpc::Mtype::REQ_RPC));
    rrq2->deserialize(msg);
    std::cout << rrq2->serialize() << std::endl;
    std::cout << rrq2->method() << std::endl;
    std::cout << rrq2->parameters()["num2"].asInt() << std::endl;
    MyRpc::RpcResponse::ptr rrq3 = MyRpc::MessageFactory::create<MyRpc::RpcResponse>();
    rrq3->SetId(Uuid::uuid());
    rrq3->setRcode(MyRpc::Rcode::RCODE_OK);
    rrq3->SetType(MyRpc::Mtype::RSP_RPC);
    Json::Value result;
    result["Res"] = 3;
    result["Tips"] = "have a good day!";
    rrq3->setResult("Have a good time");
    rrq3->check();
    std::cout << rrq3->serialize() << std::endl;
    std::cout << rrq3->GetId() << std::endl;
}
void TopicTest()
{
    MyRpc::TopicRequest::ptr tr = MyRpc::MessageFactory::create<MyRpc::TopicRequest>();
    tr->setTopicKey("Music");
    tr->setOpType(MyRpc::TopicOptype::TOPIC_PUBLISH);
    tr->setTopicMsg("就是爱你");
    std::cout << tr->serialize() << std::endl;
    MyRpc::TopicResponse::ptr tr2 = MyRpc::MessageFactory::create<MyRpc::TopicResponse>();
    tr2->SetType(MyRpc::Mtype::RSP_TOPIC);
    tr2->setRcode(MyRpc::Rcode::RCODE_OK);
    std::cout << tr2->serialize() << std::endl;
}
void ServiceTest()
{
    MyRpc::ServiceRequest::ptr tr = MyRpc::MessageFactory::create<MyRpc::ServiceRequest>();
    tr->SetType(MyRpc::Mtype::REQ_SERVICE);
    tr->setServiceOpType(MyRpc::ServiceOptype::SERVICE_REGISTRY);
    tr->setHost(std::make_pair("81.71.17.201", 8888));
    tr->setMethod("Design a goolge");
    tr->check();
    std::cout << tr->serialize() << std::endl
              << std::endl;

    MyRpc::ServiceResponse::ptr trp = MyRpc::MessageFactory::create<MyRpc::ServiceResponse>();
    std::vector<MyRpc::Address> addrs = {{"37.19.293.2", 8980}, {"90.23.11.231", 6789}, {"46.23.45.112", 6565}};
    trp->setHosts(addrs);
    trp->SetType(MyRpc::Mtype::RSP_SERVICE);
    trp->setMethod("Design a google");
    trp->setRcode(MyRpc::Rcode::RCODE_OK);
    trp->setServiceOpType(MyRpc::ServiceOptype::SERVICE_DISCOVERY);
    trp->check();
    std::cout << trp->serialize() << std::endl;
    std::cout << static_cast<int>(trp->GetType()) << std::endl;
    std::cout << trp->method() << std::endl;
    std::cout << static_cast<int>(trp->rcode()) << std::endl;
}
void onMessage(const MyRpc::ConnectionBase::ptr &conn, MyRpc::RpcResponse::ptr &msg)
{
    std::string body = msg->serialize();
    ILOG("%s,%s", "收到Message回复: ", body.c_str());
}
void onTopic(const MyRpc::ConnectionBase::ptr &conn, MyRpc::TopicResponse::ptr &msg)
{
    std::string body = msg->serialize();
    ILOG("%s,%s", "收到Topic回复: ", body.c_str());
}
void onService(const MyRpc::ConnectionBase::ptr &conn, MyRpc::ServiceResponse:ptr &msg)
{
    std::string body = msg->serialize();
    ILOG("%s,%s", "收到Service回复: ", body.c_str());
}
int main()
{
    MyRpc::Dispatcher::ptr dispatcher = std::make_shared<MyRpc::Dispatcher>();
    dispatcher->registerHandler(MyRpc::Mtype::RSP_RPC, onMessage);
    dispatcher->registerHandler(MyRpc::Mtype::RSP_TOPIC, onTopic);
    dispatcher->registerHandler(MyRpc::Mtype::RSP_SERVICE, onService);
    MyRpc::ClientBase::ptr client = MyRpc::ClientFactory::create("127.0.0.1", 9000);
    auto onmessage = std::bind(&MyRpc::Dispatcher::messageCallBack,dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
    client->SetMessageCallBack(onmessage);
    client->connect();
    std::vector<MyRpc::MessageBase::ptr> messages;
    MyRpc::RpcRequest::ptr rrq = std::dynamic_pointer_cast<MyRpc::RpcRequest>(MyRpc::MessageFactory::create(MyRpc::Mtype::REQ_RPC));
    rrq->SetId(Uuid::uuid());
    rrq->setMethod("Add");
    Json::Value params;
    params["num1"] = 1;
    params["num2"] = 4;
    rrq->setParameters(params);
    rrq->SetType(MyRpc::Mtype::REQ_RPC);
    messages.push_back(std::move(rrq));
    MyRpc::TopicRequest::ptr tr = MyRpc::MessageFactory::create<MyRpc::TopicRequest>();
    tr->setTopicKey("Music");
    tr->SetType(MyRpc::Mtype::REQ_TOPIC);
    tr->setOpType(MyRpc::TopicOptype::TOPIC_PUBLISH);
    tr->setTopicMsg("就是爱你");
    messages.push_back(std::move(tr));
    MyRpc::ServiceRequest::ptr trs = MyRpc::MessageFactory::create<MyRpc::ServiceRequest>();
    trs->SetType(MyRpc::Mtype::REQ_SERVICE);
    trs->setServiceOpType(MyRpc::ServiceOptype::SERVICE_REGISTRY);
    trs->setHost(std::make_pair("81.71.17.201", 8888));
    trs->setMethod("Design a goolge");
    messages.push_back(std::move(trs));
    for(int i = 0;i<100;++i)
    {
        i %= messages.size();
        // std::cout<<static_cast<int>(messages[i]->GetType())<<std::endl;
        client->send(messages[i++]);
        sleep(3);
    }
    return 0;
}