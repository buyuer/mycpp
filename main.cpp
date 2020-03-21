#include "mycpp.hpp"

using namespace my;

int main() {

    list<int> *a = new list_link<int>();
    list<int> *b = new list_array<int>();

    Socket sock = Socket(Socket::IPV4);

    sock.connect("127.0.0.1",6666);

    sock.io() << "1" << std::flush;

    return 0;
}
