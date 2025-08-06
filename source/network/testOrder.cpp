#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "../../json/json.h"
#include <typeinfo>
class Test{
    public:
        Test(){
            std::cout<<"Test()"<<std::endl;
        }
};
int main()
{
    Json::Value root;
    root["hello"] = 1;
    std::cout<<typeid(decltype(root["hello"])).name()<<std::endl;
    return 0;
}