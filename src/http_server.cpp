#include "http_server.h"

HTTP_Server::HTTP_Server() : logger{}, max_workers{4}, workers{}, client_queue{}, queue_mutex{}
                            ,queue_cv{} {}

void HTTP_Server::start(std::string& ip_adress, int const port_nr) {


    int socket_fd{socket(AF_INET, SOCK_STREAM,0)};
    
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

    for (int i = 0; i < max_workers; i++){
        workers.emplace_back(&HTTP_Server::worker_action, this);
    }

    std::ostringstream oss;
    oss << "Listening on " << ip_adress << ":" << port_nr;
    std::string message = oss.str();
    logger.log_info(message); 

    listen_for_clients(socket_fd);

}

void HTTP_Server::listen_for_clients(int socket_fd){
     while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
        
        if (client_fd < 0){
            std::string message{"Failed to connect to client"};
            logger.log_error(message);
            continue;
        }

        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            client_queue.push(client_fd);
        }

        queue_cv.notify_one();
    };   

}

void HTTP_Server::worker_action(){
    while (true) {
        int client_fd{};

        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this](){
                return !client_queue.empty();
            });

            client_fd = client_queue.front();
            client_queue.pop();
        }

         handle_client(client_fd);
         close(client_fd);
    }

}


void HTTP_Server::handle_client(int const client_fd)  {
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

}