#ifndef HTTP_RESPONSE
#define HTTP_RESPONSE

#include <string>
#include <unordered_map>

class HTTP_Response {
    public:
        HTTP_Response(int code, std::string body);
        int status_code;
        std::unordered_map<std::string, std::string> headers;
        std::string body;

        std::string get_response_text(int code) const;

    };

#endif