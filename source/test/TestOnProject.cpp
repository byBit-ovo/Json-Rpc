#include <iostream>
class Test{
    public:
        Test(){
            std::cout<<"Test()"<<std::endl;
        }
        ~Test(){
            std::cout<<"~Test()"<<std::endl;
        }
};
int main()
{
    Test *p = reinterpret_cast<Test*>(malloc(sizeof(Test)));
    p = new(p)Test();
    p = new(p)Test();
    delete p;
    return 0;
}