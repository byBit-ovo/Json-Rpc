#include "Util.hpp"

int main()
{
    for (int i = 0; i < 10; ++i)
    {
        std::cout << Uuid::uuid() << std::endl;
    }
    return 0;
}