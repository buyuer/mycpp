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
            this->handler = [](my::Socket client_){
                std::cout << client_.get_ip() <<":" << client_.get_port() << std::endl;
                std::string line;
                getline(client_.io(), line);
                std::cout << line << std::endl;
                client_.io() << "HTTP/1.1 200 OK\r\n\r\n" << std::flush;
                client_.close();
            };
        }

        virtual ~HttpServer() {

        }

        virtual void start() override {
            Server::start();
        }

        virtual void add_route(std::string path, route_handler routeHandler){
            route[path] = routeHandler;
        }
    };
}