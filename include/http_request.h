#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include <string>
#include <unordered_map>
#include <variant>

class HTTP_Request {
public:
    HTTP_Request();
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::unordered_map<std::string, std::string> queries;
    std::unordered_map<std::string, std::variant<std::string, int, bool, double>> body;
};

#endif