#include "requestor.hpp"
#include "rpc_caller.hpp"
std::future<int> test(){
    std::promise<int> pro;
    pro.set_value(3);
    return pro.get_future();
}
int main()
{
    std::future<int> f = test();
    std::cout<<f.get()<<std::endl;
    return 0;
}