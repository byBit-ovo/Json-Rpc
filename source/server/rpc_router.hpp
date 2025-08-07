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
                ServiceDesc(const Func_t &func,const std::string& name,const parameterType type,
                const std::unordered_map<std::string, parameterType>& parameters):
                _call(func),_name(name),_return_type(type),_parameters(parameters){}
                std::string name(){
                    return _name;
                }
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
        class ServiceDescBuilder {
            public:
                using Self = ServiceDescBuilder;
                using ptr = std::shared_ptr<Self>;
                
                Self& setMethodName(const std::string &name) {
                    _name = name;
                    return *this;
                }
                Self& setReturnType(parameterType vtype) {
                    _return_type = vtype;
                    return *this;
                }
                Self& setParamsDesc(const std::string &pname, parameterType type) {
                    _parameters.insert(std::make_pair(pname, type));
                    return *this;
                }
                Self& setCallback(const ServiceDesc::Func_t &cb) {
                    _call = cb;
                    return *this;
                }
                ServiceDesc::ptr build() {
                    return std::make_shared<ServiceDesc>(_name, _parameters, _return_type, _call);
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
                void insert(const ServiceDesc::ptr& desc){
                    std::lock_guard<std::mutex> guard(_lock);
                    _services.insert(std::make_pair(desc->name(),desc));
                }
                void remove(const std::string &name){
                    std::lock_guard<std::mutex> guard(_lock);
                    _services.erase(name);
                }
                ServiceDesc::ptr search(const std::string& name){
                    std::lock_guard<std::mutex> guard(_lock);
                    auto iter = _services.find(name);
                    if(iter != _services.end()){
                        return iter->second;
                    }
                    return ServiceDesc::ptr();
                }
            private:
                std::unordered_map<std::string,ServiceDesc::ptr> _services;
                std::mutex _lock;
        };
        //Rpc调用中转服务
        class RpcRouter{
            public:
                RpcRouter():_manager(std::make_shared<ServiceManager>())
                {}
                //该函数要注册到dispatcher的Rpc请求处，功能是查询服务、校验参数、业务处理后返回结果
               
                void registerService(const ServiceDesc::ptr& service){
                    _manager->insert(service);
                }
            private:
                void onRpcRequest(const ConnectionBase::ptr conn, RpcRequest::ptr& msg){
                    std::string name = msg->method();
                    ServiceDesc::ptr service = _manager->search(name);
                    if(service.get() == nullptr){
                        ELOG("Rpc请求方法不存在!");
                        return response(conn,Json::Value(),Rcode::RCODE_NOT_FOUND_SERVICE);
                    }
                    if(service->checkOutParameters(msg->parameters()) == false){
                        return response(conn,Json::Value(),Rcode::RCODE_INVALID_PARAMS);
                    }
                    Json::Value result;
                    if(service->call(msg->parameters(), result) == false){
                        return response(conn,Json::Value(),Rcode::RCODE_INVALID_RESULT);
                    }
                    return response(conn,result,Rcode::RCODE_OK);
                }
                void response(const ConnectionBase::ptr& conn, const Json::Value& res, Rcode rcode){
                    RpcResponse::ptr respon = MessageFactory::create<RpcResponse>();
                    respon->setRcode(rcode);
                    respon->setResult(res);
                    respon->SetType(MyRpc::Mtype::RSP_RPC);
                    respon->SetId(Uuid::uuid());
                    conn->send(respon);
                }
                ServiceManager::ptr _manager;
        };
    }
}

