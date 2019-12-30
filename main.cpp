#include "my.hpp"



int main() {
    my::Socket socket1(my::Socket::IPV4);

    socket1.bind("127.0.0.1", 7777);

    socket1.listen();

    while(true){
        my::Socket sock = socket1.accept();
        std::cout << "sock" << std::endl;
        std::string str;
        getline(std::cin, str);
        sock.io() << "Welcome" << str << std::flush;
    }

    return 0;
}
