//#include "my.hpp"

#include "src/base/buff.hpp"

int main() {

    HANDLE file = CreateFile("D:\\test.txt", GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_ALWAYS, NULL, NULL);
    my::basic_buff<char> buff(file, file);
    std::iostream io(&buff);

    io << "Hello world" << std::endl;


    return 0;
}
