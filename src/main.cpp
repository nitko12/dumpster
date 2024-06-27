#include "Server.h"
#include "Connection.h"
#include <iostream>
#include <unordered_map>
#include <vector>
#include "KQueueHandler.h"

using namespace std;

int main()
{
    Server server("0.0.0.0", 8080);
    server.start();

    KQueueHandler kqueueHandler;
    std::unordered_map<int, Connection*> connections;

    kqueueHandler.add_event(server.get_socket());

    while (true) {
        try {
            std::vector<struct kevent> events = kqueueHandler.wait_for_events();

            for (auto& event : events) {
                if (event.ident == server.get_socket()) {
                    int client_socket = server.accept_client();
                    connections[client_socket] = new Connection(client_socket);
                    kqueueHandler.add_event(client_socket);
                } else {
                    Connection* connection = connections[event.ident];
                    if (event.flags & EV_EOF) {
                        kqueueHandler.remove_event(event.ident);
                        connections.erase(event.ident);
                        delete connection;
                    } else {
                        connection->read();
                        const auto& response = connection->get_read_buffer();
                        if (!response.empty()) {
                            connection->append_to_write_buffer(response);
                        }

                        if (connection->is_closed()) {
                            kqueueHandler.remove_event(event.ident);
                            connections.erase(event.ident);
                            delete connection;
                        }

                        connection->write();
                        connection->clear_read_buffer();
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
    }


    return 0;
}
