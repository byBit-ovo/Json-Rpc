#include <iostream>
#include <arpa/inet.h>
#include <sstream>
int main()
{
    int a = 2;
    unsigned char *p = reinterpret_cast<unsigned char*>(&a);
    for(int i = 0;i < sizeof a; ++i)
    {
        printf("0x%02x\n", p[i]);
    }
    return 0;
}