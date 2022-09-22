#include "mycpp/mycpp.hpp"

int main(int argc, char **argv)
{
    mycpp::HttpServer server(10001);

    server.add_route("get_name",
                     [](mycpp::HttpRequest &, mycpp::HttpResponse &) -> void {

                     });

    server.start();
}
