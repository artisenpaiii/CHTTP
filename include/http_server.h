
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

    class HTTP_Server{

    public:
        HTTP_Server();
        void start(std::string& ip_adress, int const port_nr);

    private:
        void listen_for_clients(int socket_fd);
        void worker_action();
        void handle_client(int client_fd);

        Logger logger;
        int max_workers;
        std::vector<std::thread> workers;
        std::queue<int> client_queue;
        std::mutex queue_mutex;
        std::condition_variable queue_cv;


};

#endif