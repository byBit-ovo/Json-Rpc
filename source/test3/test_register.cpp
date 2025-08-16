#include "../server/rpc_server.hpp"

void testCommunication(){
    MyRpc::Server::RegisterServer::ptr server = std::make_shared<MyRpc::Server::RegisterServer>(9000);
    server->start();
}
int main()
{
    testCommunication();
    return 0;
}