#pragma once

#include <vector>
#include <array>
#include <unistd.h>
#include <fcntl.h>

class Connection
{
public:
    Connection() : fd(-1) {}
    Connection(int fd);
    ~Connection();

    int get_fd() const;
    bool is_closed() const;

    void read();
    void write();

    const std::vector<char>& get_read_buffer();
    void clear_read_buffer();
    void append_to_write_buffer(const std::vector<char>& data);

private:
    int fd;
    std::vector<char> read_buffer;
    std::vector<char> write_buffer;
    std::array<char, 4096> buffer;

    int set_nonblocking(int fd);
};
