#include "http_server.h"

int main(){

    HTTP_Server server{};
    
    std::string ip_addr = "127.0.0.1";
    int port_nr = 8000;
    server.start(ip_addr, port_nr);

    return 0;
}