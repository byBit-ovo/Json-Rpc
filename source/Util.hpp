#pragma once
#include <iostream>
#include <chrono>
#include <random>
#include <string>
#include <sstream>
#include <atomic>
#include <iomanip>
#include <stdio.h>
#include <time.h>
#include "../json/json.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#define LINF 0
#define LDBG 1
#define LERR 2
#define LDFT LINF
FILE *LogPlace = stdout;
#define LOG(level, format, ...)                                                                                   \
    do                                                                                                            \
    {                                                                                                             \
        if (level < LDFT)                                                                                         \
        {                                                                                                         \
            break;                                                                                                \
        }                                                                                                         \
        time_t now = time(nullptr);                                                                               \
        struct tm *cur_time = localtime(&now);                                                                    \
        fprintf(LogPlace, "%s-[%s: %d]-[%04d-%02d-%02d %02d:%02d:%02d] " format "\n", #level, __FILE__, __LINE__, \
                cur_time->tm_year + 1900, cur_time->tm_mon + 1,                                                   \
                cur_time->tm_mday, cur_time->tm_hour, cur_time->tm_min, cur_time->tm_sec, ##__VA_ARGS__);         \
    } while (0)

#define ILOG(format, ...) LOG(LINF, format, ##__VA_ARGS__)
#define DLOG(format, ...) LOG(LDBG, format, ##__VA_ARGS__)
#define ELOG(format, ...) LOG(LERR, format, ##__VA_ARGS__)
class FileLog
{
private:
    FILE *_file;

public:
    FileLog()
    {
        ILOG("filelog 构造");
        _file = fopen(".log.txt", "a");
        LogPlace = _file;
    }
    ~FileLog()
    {
        fclose(_file);
    }
};
// FileLog toFile;
class JsonUtil
{
    static std::string serialize(const Json::Value &json)
    {
        std::stringstream ss;
        Json::StreamWriterBuilder factory;
        std::unique_ptr<Json::StreamWriter> writer(factory.newStreamWriter());
        writer->write(json, &ss);
        return ss.str();
    }
    static Json::Value deserialize(const std::string &src)
    {
        Json::CharReaderBuilder factory;
        std::unique_ptr<Json::CharReader> reader(factory.newCharReader());
        Json::Value root;
        std::string err;
        if (reader->parse(src.c_str(), src.c_str() + src.size(), &root, &err) == false)
        {
            ELOG("Json deserialsize fail: %s", err.c_str());
        }
        return root;
    }
};
class Uuid
{
public:
    static std::string uuid()
    {
        std::stringstream ss;
        // 1. 构造⼀个机器随机数对象
        std::random_device rd;
        // 2. 以机器随机数为种⼦构造伪随机数对象
        std::mt19937 generator(rd());
        // 3. 构造限定数据范围的对象
        std::uniform_int_distribution<int> distribution(0, 255);
        // 4. ⽣成8个16进制随机数，01 0a a1 ...
        for (int i = 0; i < 8; i++)
        {
            if (i == 4 || i == 6)
                ss << "-";
            ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator);
        }
        ss << "-";
        // 5. 定义⼀个8字节序号，逐字节组织成为16进制数字字符的字符串
        static std::atomic<size_t> seq(1); // 00 00 00 00 00 00 00 01
        size_t cur = seq.fetch_add(1);
        for (int i = 7; i >= 0; i--)
        {
            if (i == 5)
                ss << "-";
            ss << std::setw(2) << std::setfill('0') << std::hex << ((cur >> (i * 8)) & 0xFF);
        }
        return ss.str();
    }
};

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
#define KEY_RESULT "result

    enum class MType
    {
        REQ_RPC = 0,
        RSP_RPC,
        REQ_TOPIC,
        RSP_TOPIC,
        REQ_SERVICE,
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
        RCODE_INTERNAL_ERROR
    };
    static std::unordered_map<Rcode, std::string> err_map = {
        {Rcode::RCODE_OK, "成功处理！"},
        {Rcode::RCODE_PARSE_FAILED, "消息解析失败！"},
        {Rcode::RCODE_ERROR_MSGTYPE, "消息类型错误！"},
        {Rcode::RCODE_INVALID_MSG, "⽆效消息"},
        {Rcode::RCODE_DISCONNECTED, "连接已断开！"},
        {Rcode::RCODE_INVALID_PARAMS, "⽆效的Rpc参数!"},
        {Rcode::RCODE_NOT_FOUND_SERVICE, "没有找到对应的服务！"},
        {Rcode::RCODE_INVALID_OPTYPE, "⽆效的操作类型"},
        {Rcode::RCODE_NOT_FOUND_TOPIC, "没有找到对应的主题！"},
        {Rcode::RCODE_INTERNAL_ERROR, "内部错误！"}
    };

    enum class RType
    {
        REQ_SYNC = 0,
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
        auto iter = err_map.find(code);
        if (iter == err_map.end())
        {
            return "未知错误";
        }
        return iter->second;
    }

}
