#include "../client/rpc_client.hpp"
void onResult(const Json::Value &result){
    std::cout<<"CallBack(12+16):The answer is "<<result.asInt()<<std::endl;
}
void testCommunication(){
    //注册服务注册中心的地址
    MyRpc::Client::RpcClient::ptr client = std::make_shared<MyRpc::Client::RpcClient>(true,"127.0.0.1",9000);
    MyRpc::Address host_add;
    Json::Value para;
    para["num1"] = 4;
    para["num2"] = 7;
    Json::Value ans;
    if(client->call("Add",para,ans)){
        std::cout<<"Sync(4+7):The answer is "<<ans.asInt()<<std::endl;
    }
    std::future<Json::Value> ans2;
    para["num1"] = 7;
    para["num2"] = 10;
    if(client->call("Add",para,ans2)){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout<<"ASync(7+10):The answer is "<<ans2.get().asInt()<<std::endl;
    }

    para["num1"] = 12;
    para["num2"] = 16;
    client->call("Add",para,onResult);

    sleep(30);

}
int main()
{
    testCommunication();
    return 0;
}