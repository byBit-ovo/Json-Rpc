#include "../client/rpc_client.hpp"
#include <iostream>
void UFC_Channel(const std::string &topic_name,const std::string &msg){
    std::cout<<"Recieving the topic message: "<<std::endl;
    std::cout<<"The "<<topic_name<<"Channel: "<<msg<<std::endl;
}
int main(){
    MyRpc::Client::TopicClient client("127.0.0.1",10086);
    client.createTopic("UFC");  
    bool ret = client.subscribeTopic("UFC",UFC_Channel);
    if(ret == true){
        ILOG("The topic \"UFC\" subscribed successfully!");
    }
    std::this_thread::sleep_for(std::chrono::seconds(20));
    client.shutDown();
    return 0;
}