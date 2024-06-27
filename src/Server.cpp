#include "Server.h"

Server::Server(const std::string& address, int port) : address_(address), port_(port), socket_(-1)
{}

Server::~Server()
{
    if (socket_ != -1)
    {
        close(socket_);
    }
}

void Server::start()
{
    if ((socket_ = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        throw std::runtime_error("Failed to create socket");
    }

    const int reuse = REUSEADDR;
    if (setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int)) == -1)
    {
        throw std::runtime_error("Failed to set socket options");
    }

    if (set_nonblocking(socket_) == -1)
    {
        throw std::runtime_error("Failed to set non-blocking");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port_);
    addr.sin_addr.s_addr = inet_addr(address_.c_str());

    if (bind(socket_, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        throw std::runtime_error("Failed to bind socket");
    }

    if (listen(socket_, BACKLOG) == -1)
    {
        throw std::runtime_error("Failed to listen on socket");
    }
}

int Server::get_socket() const
{
    return socket_;
}

int Server::set_nonblocking(int fd) // private, no exceptions, returns -1 on error
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL");
        return -1;
    }
    return 0;
}

int Server::accept_client()
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int client_socket = accept(socket_, (struct sockaddr*)&client_addr, &client_addr_size);
    if (client_socket == -1)
    {
        throw std::runtime_error("Failed to accept client");
    }
    return client_socket;
}
