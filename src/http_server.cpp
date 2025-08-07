#include "http_server.h"

HTTP_Server::HTTP_Server() : logger{} {}

void HTTP_Server::start(std::string& ip_adress, int const port_nr) const {


    int socket_fd = socket(AF_INET, SOCK_STREAM,0);
    
    if (socket_fd == -1){
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_adress.c_str());
    server_addr.sin_port = htons(port_nr);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0){
        return;
    }     
    
    if(listen(socket_fd, 20) < 0){
        return;
    }

    std::ostringstream oss;
    oss << "Listening on " << ip_adress << ":" << port_nr;
    std::string message = oss.str();
    logger.log_info(message); 

    handle_client(socket_fd);
}

void HTTP_Server::handle_client(int const socket_fd) const{
    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);

        char buffer[4096] = {0};
        int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);

        if (bytes_received > 0) {
            std::string message{"New Request Incomming"};
            logger.log_info(message);
        }


        const char* http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 12\r\n"
            "\r\n"
            "Hello world";

        send(client_fd, http_response, strlen(http_response), 0);

        // 4. Close the client connection
        close(client_fd);
    };

    
}