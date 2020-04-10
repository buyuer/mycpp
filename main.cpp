#include "mycpp.hpp"

using namespace my;

int main() {

    /*list<int> &a = *new list_link<int>();
    list<int> &b = *new list_array<int>();

    list_array<int> c;

    for (int i = 1; i < 100; i++) {
        c.add(i);
    }

    std::cout << *c.begin()++ << std::endl;

    for(auto &i : c){
        std::cout << i << std::endl;
    }*/

    Server httpServer(9997);

    httpServer.start();

    return 0;
}
