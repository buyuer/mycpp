module;
#include <iostream>
#include <map>
#include <string>
#include <vector>
export module mycpp.net:http;

import :server;
import :socket;

export namespace mycpp
{

class Http
{
  private:
  protected:
    std::vector<std::string>           line;
    std::map<std::string, std::string> fields;
    std::string                        body;

    bool parse(const uint8_t *data)
    {
        return false;
    }
};

class HttpRequest : public Http
{
};

class HttpResponse : public Http
{
};

class HttpServer : public Server
{

  public:
    using route_handler = void (*)(mycpp::HttpRequest &, mycpp::HttpResponse &);

  private:
    std::map<std::string, route_handler> route;

  public:
    HttpServer(int port) : Server(port)
    {
        this->handler = [](mycpp::Socket client_) {
            std::cout << client_.get_ip() << ":" << client_.get_port()
                      << std::endl;
            std::string line;
            getline(client_.io(), line);
            std::cout << line << std::endl;
            client_.io() << "HTTP/1.1 200 OK\r\n\r\n" << std::flush;
            client_.close();
        };
    }

    ~HttpServer() override
    {
    }

    void start() override
    {
        Server::start();
    }

    virtual void add_route(std::string path, route_handler routeHandler)
    {
        route[path] = routeHandler;
    }
};

class HttpClient
{
};

} // namespace mycpp
