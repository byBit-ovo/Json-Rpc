#include "Util.hpp"
class A{
    public:
        virtual void func1()
        {

        }
};
class B
{
    public:
        virtual void func2()
        {

        }
};
class C:public A ,public B
{

};
int main()
{
    C* p = new C();
    delete p;  
    return 0;
}