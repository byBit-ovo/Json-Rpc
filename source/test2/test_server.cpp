
#include "../server/rpc_server.hpp"
void Add(const Json::Value &para, Json::Value &ans){
    int num1 = para["num1"].asInt();
    int num2 = para["num2"].asInt();
    ans = num1 + num2;
}
void testCommunication(){
    MyRpc::Address host = {"81.71.17.201", 10086};
    MyRpc::Server::RpcServer::ptr server = std::make_shared<MyRpc::Server::RpcServer>(host);
    MyRpc::Server::ServiceDescBuilder::ptr builder = std::make_shared<MyRpc::Server::ServiceDescBuilder>();
    MyRpc::Server::ServiceDesc::ptr add = 
    builder->setCallback(Add).
    setMethodName("Add").
    setParamsDesc("num1",MyRpc::Server::parameterType::INTEGRAL).
    setParamsDesc("num2",MyRpc::Server::parameterType::INTEGRAL).
    setReturnType(MyRpc::Server::parameterType::INTEGRAL).
    build();
    server->registerService(add);
    server->start();
}
int main()
{
    testCommunication();
    return 0;
}