#include "mycpp/mycpp.hpp"

int main(int argc, char **argv) {
    my::HttpServer server(10001);

    server.add_route("get_name", [](my::HttpRequest &, my::HttpResponse &) -> void {

    });

    server.start();
}
