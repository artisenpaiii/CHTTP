
#ifndef HTTP_SERVER
#define HTTP_SERVER

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "logger.h"
#include <sstream>
#include <cstring>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include "http_request.h"
#include <unordered_map>
#include <unordered_set>
#include "http_response.h"
#include "http_parser.h"

using RouteHandler = std::function<HTTP_Response(const HTTP_Request&)>;

    class HTTP_Server{
    public:
        HTTP_Server();
        void start(std::string& ip_adress, int const port_nr);
        void route(std::string method, std::string end_point, RouteHandler handler);

    private:
        Logger logger;
        HTTP_Parser parser;

        int max_workers;
        std::vector<std::thread> workers;
        std::queue<int> client_queue;
        std::mutex queue_mutex;
        std::condition_variable queue_cv;

        std::unordered_map<std::string, std::unordered_map<std::string, RouteHandler>> routes;
        std::unordered_set<std::string> allowed_methods;
        std::unordered_map<int, std::string> status_text;
        void listen_for_clients(int socket_fd);
        void worker_action();
        void handle_client(int client_fd);




};

#endif