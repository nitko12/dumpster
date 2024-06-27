#pragma once

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#define REUSEADDR 1
#define BACKLOG 10

class Server {
public:
    Server(const std::string& address, int port);
    ~Server();

    void start();
    int get_socket() const;

    int accept_client();
private:
    int socket_;
    std::string address_;
    int port_;

    int set_nonblocking(int fd);
};
