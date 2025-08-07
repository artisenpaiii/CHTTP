#include "http_server.h"

int main(){

    HTTP_Server server{};

    server.route("GET", "/hello", [](const HTTP_Request& req) {
        std::cout << req.queries.at("name") << " " << req.queries.at("lastname") << std::endl;

        return HTTP_Response(200, "Hello");
    });
    
    server.route("POST", "/post", [](const HTTP_Request& req) {
    for (const auto& [key, value] : req.body) {
        std::cout << key << ": ";
        std::visit([](auto&& val) {
            std::cout << val;
        }, value);
        std::cout << std::endl;
    }

    return HTTP_Response(200, "post something");
});




    std::string ip_addr = "127.0.0.1";
    int port_nr = 8000;
    server.start(ip_addr, port_nr);

    return 0;
}