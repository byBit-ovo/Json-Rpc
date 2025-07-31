#pragma once
#include "abstract.hpp"
#include "enum.hpp"
#include "Util.hpp"

namespace MyRpc
{
    class JsonMessage :public MessageBase
    {
        public:
            //length - mtype - idlength - id - body
            using ptr = std::shared_ptr<JsonMessage>;
            virtual std::string serialize()override
            {
                return JsonUtil::serialize(_body);     
            }
            virtual bool deserialize(const std::string& msg)
            {
                _body = JsonUtil::deserialize(msg);
                return true;
            }
        protected:
            Json::Value _body;
    };

    class JsonRequest :public JsonMessage
    {
        public:
            using ptr = std::shared_ptr<JsonRequest>;
            //check函数由具体的JsonRequest的派生类实现
    };

    class JsonResponse :public JsonMessage
    {
        public:
            using ptr = std::shared_ptr<JsonResponse>;
            virtual bool check() override
            {
                if(_body[KEY_RCODE].isNull())
                {
                    ELOG("缺失响应状态码!");
                    return false;
                }
                if(_body[KEY_RCODE].isIntegral() == false)
                {
                    ELOG("响应状态码类型错误!");
                    return false;
                }
                return true;
            }
    };

    class RpcRequest: public JsonRequest
    {
        public:
            using ptr = std::shared_ptr<RpcRequest>;
            virtual bool check() override
            {
                if(_body[KEY_METHOD].isNull()){
                    ELOG("Rpc请求方法缺失!");
                    return false;
                }
                if(_body[KEY_METHOD].isString()==false)
                {
                    ELOG("Rpc请求方法类型错误!");
                    return false;
                }
                if(_body[KEY_PARAMS].isNull())
                {
                    ELOG("Rpc请求参数缺失!");
                    return false;
                }
                if(_body[KEY_PARAMS].isObject()==false){
                    ELOG("Rpc请求参数类型错误!");
                    return false;
                }
                return true;
            }
            std::string method()
            {
                return _body[KEY_METHOD].asString();
            }
            void setMethod(const std::string& method)
            {
                _body[KEY_METHOD] = method;
            }
            Json::Value parameters(){
                return _body[KEY_PARAMS];
            }
            void setParameters(const Json::Value& params)
            {
                _body[KEY_PARAMS] = params; 
            }
    };
    class TopicRequest :public JsonRequest
    {
        public:
            using ptr = std::shared_ptr<TopicRequest>;
            virtual bool check() override
            {
                if(_body[KEY_TOPIC_KEY].isNull()){
                    ELOG("主题名缺失!");
                    return false;
                }
                if(_body[KEY_TOPIC_KEY].isString() == false){
                    ELOG("主题名类型错误!");
                    return false;
                }
                if(_body[KEY_OPTYPE].isNull()){
                    ELOG("主题操作缺失!");
                    return false;
                }
                if(_body[KEY_OPTYPE].isIntegral() == false){
                    ELOG("主题操作类型错误!");
                    return false;
                }
                if(_body[KEY_OPTYPE].asInt() == (int)TopicOptype::TOPIC_PUBLISH)
                return true;
            }

    };
    class ServiceRequest :public JsonRequest
    {
        public:
            using ptr = std::shared_ptr<ServiceRequest>;
    };

}