#include "/usr/include/jsoncpp/json/json.h"
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
int main()
{
    Json::Value root;
    root["format"] = ".png";
    root["code"] = "utf-8";

    std::string str = serialize(root);
    std::cout<<str<<std::endl;
    return 0;
}