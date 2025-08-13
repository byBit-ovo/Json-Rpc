#pragma once 
#include "network.hpp"
namespace MyRpc{
    class CallBackBase
    {
        public:
            using ptr = std::shared_ptr<CallBackBase>;
            virtual void call(const ConnectionBase::ptr& conn, MessageBase::ptr& msg)=0;

    };
    template<class MessageT>
    class CallBack: public CallBackBase
    {
        public:
            using ptr = std::shared_ptr<CallBack<MessageT>>;
            using Func_t = std::function<void(const ConnectionBase::ptr&, std::shared_ptr<MessageT>&)>;
            CallBack(const Func_t& func):_func(func){}
            virtual void call(const ConnectionBase::ptr& conn, MessageBase::ptr& msg)override{ 
                auto messg = std::dynamic_pointer_cast<MessageT>(msg);
                _func(conn,messg);
            }
        private:
            Func_t _func;

    };
    //接受到消息后，根据消息类型，派发到不同的接口
    class Dispatcher
    {
        public: 
            using ptr = std::shared_ptr<Dispatcher>;
            //在注册回调函数时，已经明确消息类型，为了方便在回调函数中调用子类接口，可以明确用参数为子类的函数接口，为了兼容，这里用模板
            //此处的MessageT，在本项目中为请求或响应消息类型
            template<typename MessageT>
            void registerHandler(Mtype mtype, typename CallBack<MessageT>::Func_t func){
                std::lock_guard<std::mutex> guard(_lock);
                //insert, 如果存在，则什么都不做，但是[]会修改原内容
                typename CallBack<MessageT>::ptr functor = std::make_shared<CallBack<MessageT>>(func);
                _dispatcher.insert(std::make_pair(mtype, functor));
            }
            //此函数需要注册到MuduoServer或者MuduoClient的_message_call_back中
            void messageCallBack(const ConnectionBase::ptr& conn, MessageBase::ptr& msg){
                std::lock_guard<std::mutex> guard(_lock);
                Mtype mtype = msg->GetType();
                auto iter = _dispatcher.find(mtype);
                if(iter == _dispatcher.end()){
                    ELOG("未在 dispatcher 中找到对应处理函数");
                    conn->shutDown(); 
                    return;
                }
                iter->second->call(conn,msg);
            } 
        private:
            std::mutex _lock;
            //设计使， value能够存储不同的函数类型，利用多态达到目的
            //根据消息类型 调用对应的回调函数...
            std::unordered_map<Mtype, CallBackBase::ptr> _dispatcher;
    };
}