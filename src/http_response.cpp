#include "http_response.h"

HTTP_Response::HTTP_Response(int code, std::string body) : 
status_code{code}, headers{}, body{body} {}
