#include "Connection.h"
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>

Connection::Connection(int fd)
    : fd(fd)
{
    if (set_nonblocking(fd) == -1)
    {
        throw std::runtime_error("Failed to set non-blocking mode");
    }

    read_buffer.reserve(4096);
    write_buffer.reserve(4096);
}

Connection::~Connection()
{
    std::cout << "Closing connection: " << fd << std::endl;
    if (!is_closed())
    {
        close(fd);
    }
}

int Connection::get_fd() const
{
    return fd;
}

bool Connection::is_closed() const
{
    return fd == -1;
}

void Connection::read()
{
    if (is_closed())
    {
        throw std::runtime_error("Connection is closed");
    }

    char buffer[4096];
    ssize_t len = 0;

    while ((len = ::read(fd, buffer, sizeof(buffer))) > 0)
    {
        read_buffer.insert(read_buffer.end(), buffer, buffer + len);
    }

    if (len == -1)
    {
        if (errno != EAGAIN && errno != EWOULDBLOCK)
        {
            close(fd);
            fd = -1;
        }
    }
    else if (len == 0)
    {
        close(fd);
        fd = -1;
    }
}

void Connection::write()
{
    if (is_closed())
    {
        throw std::runtime_error("Connection is closed");
    }

    ssize_t len = 0;

    while (!write_buffer.empty() && (len = ::write(fd, write_buffer.data(), std::min(write_buffer.size(), static_cast<size_t>(65536)))) > 0)
    {
        write_buffer.erase(write_buffer.begin(), write_buffer.begin() + len);

        std::cout << "Wrote " << len << " bytes" << std::endl;
    }

    if (len == -1 && errno != EAGAIN && errno != EWOULDBLOCK)
    {
        close(fd);
        fd = -1;
    }
}

const std::vector<char>& Connection::get_read_buffer()
{
    return read_buffer;
}

void Connection::clear_read_buffer()
{
    read_buffer.clear();
}

void Connection::append_to_write_buffer(const std::vector<char>& data)
{
    write_buffer.insert(write_buffer.end(), data.begin(), data.end());
}

int Connection::set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
    {
        perror("fcntl F_GETFL");
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl F_SETFL");
        return -1;
    }
    return 0;
}
