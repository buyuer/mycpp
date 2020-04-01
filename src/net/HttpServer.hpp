#pragma once

#include "Server.hpp"
#include "Http.hpp"

namespace my {

    class HttpServer : public Server{

    public:

        HttpServer(int port) : Server(port) {

        }

        void start() override {

            Server::start();

        }
    };
}