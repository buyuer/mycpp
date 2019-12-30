#include <netinet/in.h>
#include "base/base.hpp"

namespace my{

    class Socket{

    private:

        struct element{
            int rec_num;
#ifdef USE_LINUX
            int socket_fd;
            sockaddr_in6 addr;
#endif
            socketbuff sockbuf;
            std::iostream io;
            element():rec_num(0), socket_fd(0), io(&sockbuf){
                memset(&addr, 0, sizeof(sockaddr_in6));
            }
            ~element(){
                //由于close socket后，sokbuf才被析构，在析构时，sync要被调用，此时socket已经closed，所以这里要先同步一次，清空缓冲区。
                //sockbuf.sync();
#ifdef USE_LINUX
                close(socket_fd);
                sockbuf.dis_init();
#endif
            }
        };

        element* sock;

    public:

        Socket(const Socket& s){
            this->sock = s.sock;
            this->sock->rec_num += 1;
        }

        Socket(Socket&& s){
            this->sock = s.sock;
            this->sock->rec_num += 1;
        }

        Socket&operator=(const Socket& s){
            this->sock->rec_num -= 1;
            this->sock = s.sock;
            this->sock->rec_num += 1;
            return *this;
        }

        Socket&operator=(Socket&& s){
            this->sock->rec_num -= 1;
            this->sock = s.sock;
            this->sock->rec_num += 1;
            return *this;
        }

        ~Socket(){
            sock->rec_num -= 1;
            if(sock->rec_num <= 0){
                delete sock;
            }
        }

#ifdef USE_LINUX
    private:
        explicit Socket(const int& s):sock(new element){
            sock->socket_fd = s;
            sock->sockbuf.init(sock->socket_fd, sock->socket_fd);
            sock->rec_num = 1;
        }

    public:
        enum TYPE:int {
            IPV4 = AF_INET,
            IPV6 = AF_INET6,
            TCP = IPPROTO_TCP,
            UDP = IPPROTO_UDP,
            STREAM = SOCK_STREAM,
            DGRAM  = SOCK_DGRAM,
        };

        Socket(TYPE inet, TYPE type = STREAM, TYPE protocol = TCP):sock(nullptr){
            sock = new element();
            sock->socket_fd = ::socket(inet, type, protocol);
            if(sock->socket_fd == -1){
                perror("socket failed");
                exit(0);
            }
            sock->sockbuf.init(sock->socket_fd, sock->socket_fd);
            sock->addr.sin6_family = inet;
            sock->rec_num = 1;
        }

        void bind(std::string ip, int port){
            if( ((sockaddr*)&sock->addr)->sa_family == IPV4 ){
                sockaddr_in& addr = *(sockaddr_in*)(&sock->addr);
                addr.sin_family = IPV4;
                addr.sin_port = ::htons(port);
                addr.sin_addr.s_addr = ::inet_addr(ip.data());

                if(::bind(sock->socket_fd, (sockaddr*)&addr, sizeof(sockaddr_in)) == -1){
                    perror("bind failed");
                    exit(0);
                }
            }
            else if(((sockaddr*)&sock->addr)->sa_family == IPV6){
                sockaddr_in6& addr = *(sockaddr_in6*)(&sock->addr);
                addr.sin6_family = IPV6;
                addr.sin6_port = ::htons(port);
                if(inet_pton(IPV6,ip.data(),addr.sin6_addr.__in6_u.__u6_addr8) < 0 ){
                    perror("inet_pton failed");
                    exit(0);
                }

                if(::bind(sock->socket_fd, (sockaddr*)&addr, sizeof(sockaddr_in6)) == -1){
                    perror("bind failed");
                    exit(0);
                }
            }
        }

        void listen(int backlog = SOMAXCONN){
            if(::listen(sock->socket_fd, backlog) == -1){
                perror("listen failed");
                exit(0);
            }
        }

        my::Socket accept(){
            int s = ::accept(sock->socket_fd, nullptr, NULL);
            return Socket(s);
        }

        void connect(std::string ip, int port){

            sockaddr_in addr;
            memset(&addr, 0, sizeof(sockaddr_in));

            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr(ip.data());
            addr.sin_port = htons(port);

            if(::connect(sock->socket_fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
                perror("connect failed");
                exit(0);
            }
        }
#endif
        std::iostream& io(){
            return sock->io;
        }
    };
}