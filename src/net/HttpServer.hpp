#pragma once

#include "Server.hpp"
#include "Http.hpp"

namespace my {

    class HttpServer : public Server {

    public:

        using route_handler = void (*)(my::HttpRequest &, my::HttpResponse &);

    private:

        std::map<std::string, route_handler> route;


    public:

        HttpServer(int port) : Server(port) {

        }

        virtual ~HttpServer() {

        }

        virtual void start() override {
            Server::start();
        }

        virtual void set_route(std::string path, route_handler routeHandler){
            route[path] = routeHandler;
        }
    };
}