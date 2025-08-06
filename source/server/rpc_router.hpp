#include "../network/message.hpp"
#include "../network/network.hpp"

namespace MyRpc
{
    namespace Server{
        //枚举参数类型
        enum class parameterType{
            BOOL=0,
            INTEGRAL,
            NUMERIC,
            STRING,
            ARRAY,
            OBJECT
        };
        //某一种服务调用的具体描述
        class ServiceDesc{
            public:
                using ptr = std::shared_ptr<ServiceDesc>;
                using ParameterDesc = std::pair<std::string, parameterType>;
                using Func_t = std::function<void(const Json::Value&, Json::Value&)>;
                ServiceDesc(Func_t &&func,const std::string&& name,parameterType type,
                const std::unordered_map<std::string, parameterType>&& parameters):
                _call(func),_name(name),_return_type(type),_parameters(parameters){}

                bool checkOutParameters(const Json::Value& parameters){
                    //检验参数名以及参数类型正确性  
                    if(parameters.size() != _parameters.size()){
                        ELOG("Rpc请求中参数个数错误");
                        return false;
                    }
                    for(const auto& desc: _parameters){
                        if(parameters.isMember(desc.first) == false){
                            ELOG("Rpc请求中参数错误");
                            return false;
                        }
                        // desc.second == parameters[desc.first].isxxx()
                        if(checkType(desc.second, parameters[desc.first]) == false){
                            ELOG("Rpc请求中参数类型错误");
                            return false;
                        }
                    }
                    return true;
                }

                bool call(const Json::Value& prams, Json::Value& result){
                    _call(prams, result);
                    if(checkReturnType(result) == false){
                        ELOG("Rpc响应结果类型错误");
                        return false;
                    }
                    return true;
                }
            private:
                bool checkReturnType(const Json::Value& rt){
                    return checkType(_return_type, rt);
                }
                bool checkType(parameterType type, const Json::Value& pram){
                    switch(type){
                        case parameterType::BOOL: return pram.isBool();
                        case parameterType::INTEGRAL: return pram.isIntegral();
                        case parameterType::NUMERIC: return pram.isNumeric();
                        case parameterType::STRING: return pram.isString();
                        case parameterType::ARRAY: return pram.isArray();
                        case parameterType::OBJECT: return pram.isObject();
                    }
                    return false;
                }
                Func_t _call;
                std::string _name;
                parameterType _return_type;
                std::unordered_map<std::string, parameterType> _parameters;
        };
        //建造者模式
        class ServiceDescFactory {
            public:
                void setMethodName(const std::string &name) {
                    _name = name;
                }
                void setReturnType(parameterType vtype) {
                    _return_type = vtype;
                }
                void setParamsDesc(const std::string &pname, parameterType type) {
                    _parameters.insert(std::make_pair(pname, type));
                }
                void setCallback(const ServiceDesc::Func_t &cb) {
                    _call = cb;
                }
                ServiceDesc::ptr build() {
                    return std::make_shared<ServiceDesc>(std::move(_name), 
                        std::move(_parameters), _return_type, std::move(_call));
                }
            private:
                ServiceDesc::Func_t _call;
                std::string _name;
                parameterType _return_type;
                std::unordered_map<std::string, parameterType> _parameters;
        };
        //服务管理单元，用于服务的增删查
        class ServiceManager{
            public:
                using ptr = std::shared_ptr<ServiceManager>;
                void add(){}
                void remove(){}
                void search(){}
            private:
                std::unordered_map<std::string,ServiceDesc::ptr> _services;
                std::mutex _lock;
        };
        //Rpc调用中转服务
        class RpcRouter{
            public:
                RpcRouter()
                {}
                //该函数要注册到dispatcher的Rpc请求处，功能是查询服务、校验参数、业务处理后返回结果
                void onRpcRequest(const ConnectionBase::ptr conn, RpcRequest::ptr& msg){

                }
                void registerService(const ServiceDesc::ptr& service){

                }
            private:
                ServiceManager::ptr _manager;
        };
    }
}

