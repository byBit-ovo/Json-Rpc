
#include "../server/rpc_server.hpp"
void Add(const Json::Value &para, Json::Value &ans){
    int num1 = para["num1"].asInt();
    int num2 = para["num2"].asInt();
    ans = num1 + num2;
}
void testCommunication(){
    // MyRpc::Address host = {"81.71.17.201", 10086};
    MyRpc::Address host = {"127.0.0.1", 10086};

    MyRpc::Address host_register ={"127.0.0.1",9000};
    //开启服务注册
    MyRpc::Server::RpcServer::ptr server = std::make_shared<MyRpc::Server::RpcServer>(host,true,host_register);
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