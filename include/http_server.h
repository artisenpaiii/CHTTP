
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

    class HTTP_Server{

    public:
        HTTP_Server();
        void start(std::string& ip_adress, int const port_nr) const;

    private:
        void handle_client(int socket_fd) const;
        Logger logger;
};

#endif