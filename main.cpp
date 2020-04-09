#include "mycpp.hpp"

using namespace my;

int main() {

    list<int> &a = *new list_link<int>();
    list<int> &b = *new list_array<int>();

    list_link<int> c;

    c.begin();

    Socket sock = Socket(Socket::IPV4);

    //sock.connect("127.0.0.1",6666);

    //sock.io() << "1" << std::flush;

    Server httpServer(9999);

    httpServer.start();

    return 0;
}
