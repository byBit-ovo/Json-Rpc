#pragma once
#include <unordered_map>
#include <string>
namespace MyRpc
{
#define KEY_METHOD "method"
#define KEY_PARAMS "parameters"
#define KEY_TOPIC_KEY "topic_key"
#define KEY_TOPIC_MSG "topic_msg"
#define KEY_OPTYPE "optype"
#define KEY_HOST "host"
#define KEY_HOST_IP "ip"
#define KEY_HOST_PORT "port"
#define KEY_RCODE "rcode"
#define KEY_RESULT "result"

    enum class Mtype
    {
        REQ_RPC = 0,
        REQ_SERVICE,
        REQ_TOPIC,
        RSP_RPC,
        RSP_TOPIC,
        RSP_SERVICE
    };
    enum class Rcode
    {
        RCODE_OK = 0,
        RCODE_PARSE_FAILED,
        RCODE_ERROR_MSGTYPE,
        RCODE_INVALID_MSG,
        RCODE_DISCONNECTED,
        RCODE_INVALID_PARAMS,
        RCODE_NOT_FOUND_SERVICE,
        RCODE_INVALID_OPTYPE,
        RCODE_NOT_FOUND_TOPIC,
        RCODE_INTERNAL_ERROR,
        RCODE_INVALID_RESULT
    };
    static std::unordered_map<Rcode, std::string> RcodeDesc = {
        {Rcode::RCODE_OK, "成功处理！"},
        {Rcode::RCODE_PARSE_FAILED, "消息解析失败！"},
        {Rcode::RCODE_ERROR_MSGTYPE, "消息类型错误！"},
        {Rcode::RCODE_INVALID_MSG, "⽆效消息"},
        {Rcode::RCODE_DISCONNECTED, "连接已断开！"},
        {Rcode::RCODE_INVALID_PARAMS, "⽆效的Rpc参数!"},
        {Rcode::RCODE_NOT_FOUND_SERVICE, "没有找到对应的服务！"},
        {Rcode::RCODE_INVALID_OPTYPE, "⽆效的操作类型"},
        {Rcode::RCODE_NOT_FOUND_TOPIC, "没有找到对应的主题！"},
        {Rcode::RCODE_INTERNAL_ERROR, "内部错误！"},
        {Rcode::RCODE_INVALID_RESULT, "结果类型错误!"}
    };

    enum class ReqType
    {
        REQ_ASYNC = 0,
        REQ_CALLBACK
    };

    enum class TopicOptype
    {
        TOPIC_CREATE = 0,
        TOPIC_REMOVE,
        TOPIC_SUBSCRIBE,
        TOPIC_CANCEL,
        TOPIC_PUBLISH,
        SERVICE_UNKNOW
    };
    enum class ServiceOptype
    {
        SERVICE_REGISTRY = 0,
        SERVICE_DISCOVERY,
        SERVICE_ONLINE,
        SERVICE_OFFLINE,
        SERVICE_UNKNOW
    };
    std::string ErrReason(Rcode code)
    {
        auto iter = RcodeDesc.find(code);
        if (iter == RcodeDesc.end())
        {
            return "未知错误";
        }
        return iter->second;
    }

}
