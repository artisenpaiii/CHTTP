#ifndef HTTP_PARSER
#define HTTP_PARSER

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

#include "http_request.h"
#include "http_response.h"

class HTTP_Parser {
private:
    std::string lower_case(std::string s);
    void parse_header(std::istringstream& request_stream, HTTP_Request& req);
    void parse_body(std::string& body, HTTP_Request& req);
    void parse_json(HTTP_Request& req, std::string& json_string);

public:
    HTTP_Parser();

    HTTP_Request parse_request(int bytes_received, char buffer[]);
    std::string generate_response(HTTP_Response& response, std::string code_text);
    std::string default_404();
};

#endif
