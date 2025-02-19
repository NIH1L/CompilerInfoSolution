#include <json/json.h>
#include <iostream>
#include <sstream>

int main() {
    std::string jsonStr = R"({"exePath": "C:\\test.exe"})";
    Json::Value root;
    Json::CharReaderBuilder reader;
    std::string errors;
    std::istringstream s(jsonStr);

    if (!Json::parseFromStream(reader, s, &root, &errors)) {
        std::cerr << "JSON 解析错误: " << errors << std::endl;
        return 1;
    }

    std::cout << "exePath: " << root["exePath"].asString() << std::endl;
    return 0;
}