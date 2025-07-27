#include <pthread.h>
#include <iostream>
#include <future>

void* ThreadEntry(void* arg){
    std::cout<<"I come into ThreadEntry"<<std::endl;    
    return nullptr;
}
int multiple(int a,int b){
    std::cout<<"multiple"<<std::endl;
    return a*b;
}
void TestFuture(){
    //std::launch::async 表示异步调用，创建新线程去执行任务
    //std::launch::deferred 表示延迟调用，只有调用future对象的get()接口时才会去执行任务，并不创建新线程
    std::future<int> fu = std::async(std::launch::deferred,multiple,3,6); 
    std::cout<<fu.get()<<std::endl;
}
void TestPackagedTask()
{
    std::packaged_task<int(int,int)> ptask(multiple);
    std::future<int> fu = ptask.get_future();
    ptask(5,6);

    std::cout<<fu.get()<<std::endl;
}
int main()
{    
    TestPackagedTask();
    std::thread t(multiple);
    return 0;
}