#pragma once 
#include "network.hpp"
namespace MyRpc{
    class CallBack
    {

    };
    class Dispatcher
    {
        public: 
            using Func_t = MessageCallBack;
            using ptr = std::shared_ptr<Dispatcher>;
            //在注册回调函数时，已经明确消息类型，所以可以明确用参数为子类的函数接口，为了兼容，这里用模板
            template<typename Tfunc>
            void registerHandler(Mtype mtype, const Tfunc& func){
                std::lock_guard<std::mutex> guard(_lock);
                //insert, 如果存在，则什么都不做，但是[]会修改原函数
                _dispatcher.insert(std::make_pair(mtype, func));
            }
            void messageCallBack(const ConnectionBase::ptr& conn, MessageBase::ptr& msg){
                std::lock_guard<std::mutex> guard(_lock);
                Mtype mtype = msg->GetType();
                auto iter = _dispatcher.find(mtype);
                if(iter == _dispatcher.end()){
                    ELOG("未在 dispatcher 中找到对应处理函数");
                    conn->shutDown(); 
                    return;
                }
                iter->second(conn, msg);
            } 
        private:
            std::mutex _lock;
            std::unordered_map<Mtype, CallBack> _dispatcher;
    };
}