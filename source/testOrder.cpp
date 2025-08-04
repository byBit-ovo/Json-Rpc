#include <iostream>
#include <arpa/inet.h>
#include <sstream>
int main()
{
    std::stringstream ss;
    const char *s = "hello";
    ss << 32 << s << 9.23;
    std::cout<<ss.str()<<std::endl;
    return 0;
}