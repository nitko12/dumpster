#pragma once

#include <sys/event.h>
#include <unistd.h>
#include <vector>

#define MAX_EVENTS 32

class KQueueHandler
{
public:
    KQueueHandler();
    ~KQueueHandler();

    void add_event(int fd);
    void remove_event(int fd);
    const std::vector<struct kevent>& wait_for_events();

private:
    int kq;
    std::vector<struct kevent> events;
};
