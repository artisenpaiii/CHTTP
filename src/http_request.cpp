#include "http_request.h"

HTTP_Request::HTTP_Request() :
    method{}, path{}, headers{}, queries{}, body{} {}