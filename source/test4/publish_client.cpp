#include "../client/rpc_client.hpp"

int main(){
    MyRpc::Client::TopicClient client("127.0.0.1",10086);
    bool ret = client.createTopic("UFC");
    if(ret == false){
        return 1;
    }
    //waiting for sub-client to subscribe
    std::this_thread::sleep_for(std::chrono::seconds(10));
    client.publish("UFC","Olivera beats Dustin porioier!");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    client.publish("UFC","张伟 vs. Johnson —— 史诗对决即将上演!");
    std::this_thread::sleep_for(std::chrono::seconds(3));

    client.publish("UFC","你可以跑，但你躲不掉八角笼");
    std::this_thread::sleep_for(std::chrono::seconds(5));

    client.publish("UFC","他不是来打比赛的，他是来统治的!");
    std::this_thread::sleep_for(std::chrono::seconds(100));
    client.shutDown();
    return 0;
}