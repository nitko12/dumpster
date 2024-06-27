#include "KQueueHandler.h"
#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>

KQueueHandler::KQueueHandler()
{
    if((kq = kqueue()) == -1)
        throw std::runtime_error("kqueue() failed");

    events.reserve(MAX_EVENTS);
}

KQueueHandler::~KQueueHandler()
{
    close(kq);
}

void KQueueHandler::add_event(int fd)
{
    std::cout << "reg " << fd << std::endl;

    struct kevent ev;
    EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &ev, 1, NULL, 0, NULL) == -1)
    {
        throw std::runtime_error("kevent() failed to add event");
    }
}

void KQueueHandler::remove_event(int fd)
{
    struct kevent event;
    EV_SET(&event, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

    std::cout << "dereg " << fd << std::endl;
    if(kevent(kq, &event, 1, NULL, 0, NULL) == -1)
    {
        throw std::runtime_error("kevent() failed to remove event");
    }
}

const std::vector<struct kevent>& KQueueHandler::wait_for_events()
{
    struct timespec timeout = {0, 500000000}; // 0.5 seconds
    events.resize(MAX_EVENTS);
    int nevents = kevent(kq, NULL, 0, events.data(), MAX_EVENTS, &timeout);
    if(nevents == -1)
    {
        if(errno == EINTR)
        {
            events.clear();
            return events;
        }
        throw std::runtime_error("kevent() failed to wait for events");
    }
    events.resize(nevents);
    return events;
}
