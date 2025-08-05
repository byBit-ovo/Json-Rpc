#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <unordered_map>
int main()
{
    std::unordered_map<int, int> map;
    map[1] = 3;
    map.insert(std::make_pair(1,5));
    std::cout<<map[1];
    return 0;
}