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
            this->handler = [](std::iostream& io_, const sock_info& sockInfo){
                std::cout << sockInfo.ip <<":" << sockInfo.port << std::endl;
                std::string line;
                getline(io_, line);
                std::cout << line << std::endl;
                io_ << "HTTP/1.1 200 OK\r\n\r\n" << std::flush;
                sockInfo.close();
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