#include "http_server.h"

HTTP_Server::HTTP_Server() : logger{}, parser{}, max_workers{4}, workers{}, client_queue{}, queue_mutex{}
                            ,queue_cv{}, routes{}, allowed_methods{"GET", "POST", "PUT", "DELETE"} {
                                status_text[200] = "OK";
                            }

void HTTP_Server::start(std::string& ip_adress, int const port_nr) {


    int socket_fd{socket(AF_INET, SOCK_STREAM,0)};
    
    if (socket_fd == -1){
        return;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip_adress.c_str());
    server_addr.sin_port = htons(port_nr);

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
         logger.log_error("OPTION ERROR SOCKET");
        close(socket_fd);
        return;
    }

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) < 0){
        logger.log_error("BINDING SOCKET");
        close(socket_fd);
        return;
    }     
    
    if(listen(socket_fd, 20) < 0){
        logger.log_error("LISTEN ERROR");
        close(socket_fd);
        return;
    }

    for (int i = 0; i < max_workers; i++){
        workers.emplace_back(&HTTP_Server::worker_action, this);
    }

    std::ostringstream oss;
    oss << "Listening on " << ip_adress << ":" << port_nr;
    logger.log_info(oss.str()); 
    listen_for_clients(socket_fd);
    close(socket_fd);

}

void HTTP_Server::route(std::string method, std::string end_point, RouteHandler handler){
    if (allowed_methods.find(method) == allowed_methods.end()) {
        std::ostringstream oss;
        oss << "Invalid Method: " << method;
        logger.log_error(oss.str());
        return;
    }

    routes[method][end_point] = handler;
}

void HTTP_Server::listen_for_clients(int socket_fd){
     while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len{sizeof(client_addr)};
        int client_fd{accept(socket_fd, (struct sockaddr*)&client_addr, &client_len)};
        
        if (client_fd < 0){
            logger.log_error("Failed to connect to client");
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

        // if (bytes_received > 0) {

        //     logger.log_info("New Request Incomming");
        // }
        HTTP_Request request{parser.parse_request(bytes_received, buffer)};
        RouteHandler handler{routes[request.method][request.path]};

        if(!handler){
            std::string default_reponse{parser.default_404()};
            send(client_fd, default_reponse.c_str(), default_reponse.size(), 0);
            return;
        }

        HTTP_Response response{handler(request)};
        std::string status{status_text[response.status_code]};

        std::string http_reponse = parser.generate_response(response, status);
        logger.log_method(request.method, status);

        send(client_fd, http_reponse.c_str(), http_reponse.size() , 0);


}