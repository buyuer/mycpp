#pragma once

#include "Socket.hpp"

namespace my {

    class HttpServer {

    private:
        Socket sock;

    public:

        HttpServer(int port) : sock(Socket::IPV4) {
            sock.bind("127.0.0.1", port);
        }

        void start() {

            sock.listen();

            while (true) {

            }
        }
    };
}