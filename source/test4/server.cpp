#include "../server/rpc_server.hpp"

int main(){
    MyRpc::Server::TopicServer server(10086);
    server.start();
    return 0;
}

