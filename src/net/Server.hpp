#pragma once

#include "net/Socket.hpp"

namespace my {
    class Server {

    private:
        Socket sock;

#ifdef USE_LINUX
        int epfd;
        epoll_event *evs;
        const size_t MAX_EVENT = 4096;
#endif

    public:
        Server(int port) :
                sock(Socket::IPV4), epfd(-1), evs(nullptr) {
            sock.bind("127.0.0.1", port);
            if ((epfd = ::epoll_create(MAX_EVENT)) == -1) {
                perror("epoll_create failed");
                exit(0);
            }
            evs = new epoll_event[MAX_EVENT];
        }


        ~Server() {
            if (evs != nullptr) {
                delete[] evs;
            }
        }

        void start() {
            sock.listen();

            epoll_event ev;
            ev.events = ::EPOLLIN | ::EPOLLET | ::EPOLLERR;
            ev.data.fd = sock.get_socket();

            if (::epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
                perror("epoll_ctl failed");
                exit(0);
            }

            while (true) {
                int con = ::epoll_wait(epfd, evs, MAX_EVENT, -1);
                for (int i = 0; i < con; i++) {
                    if (evs[i].events & ::EPOLLIN && evs[i].data.fd == sock.get_socket()) {
                        sockaddr_in client;
                        socklen_t len = sizeof(client);
                        int cfd = ::accept(sock.get_socket(), (sockaddr *) (&client), &len);
                        if (cfd == -1) {
                            perror("accept failed");
                        }
                        printf("get connection \n");
                    }
                }
            }

        }
    };
}