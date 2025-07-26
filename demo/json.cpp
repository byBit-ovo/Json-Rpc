#include "../json/json.h"
#include <iostream>
#include <sstream>
void test(){
    Json::Value student;
    student["Name"] = "John";
    student["hobby"] = "football";
    student["scores"].append(89.2);
    student["scores"].append(91);
    student["scores"].append(73);
    student["scores"].append("Ohhhh");
    student["self"] = student;
    Json::StreamWriterBuilder factory;
    Json::StreamWriter* writer = factory.newStreamWriter();
    writer->write(student,&std::cout);
    delete writer; 
}
std::string serialize(const Json::Value& json){
    std::stringstream ss;
    Json::StreamWriterBuilder factory;
    std::unique_ptr<Json::StreamWriter> writer(factory.newStreamWriter());
    writer->write(json,&ss);
    return ss.str();
}
Json::Value deserialize(const std::string& src)
{
    Json::CharReaderBuilder factory;
    std::unique_ptr<Json::CharReader> reader(factory.newCharReader());
    Json::Value root;
    std::string err;
    reader->parse(src.c_str(),src.c_str() + src.size(),&root,&err);
    return root;
}

int main()
{
    // Json::Value root;
    // root["format"] = ".png";
    // root["code"] = "utf-8";
    // std::string str = serialize(root);
    // std::cout<<str<<std::endl;
    std::string s = "{\"Name\": \"Jack\",\"Age\": 20,\"Hobby\": \"football\"}";
    Json::Value val = deserialize(s);
    std::cout<<s<<std::endl;
    std::cout<<val["Name"].asString()<<std::endl;
    std::cout<<val["Age"].asInt()<<std::endl;
    std::cout<<val["Hobby"].asString()<<std::endl;
    return 0;
}