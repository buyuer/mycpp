#pragma once

#include "net/Socket.hpp"

namespace my {

    class Server {

    private:

        struct element {
            std::mutex m;
            std::condition_variable cv;
            std::thread t;
            bool flag;

            element() : flag(false) {}

            element(const element &h) {
                this->flag = h.flag;
            }

            element &operator=(const element &h) {
                this->flag = h.flag;
                return *this;
            }
        };


        using func = void (*)(element *, const int);

        func handler = [](element *ele_, const int sock_) -> void {
            std::unique_lock<std::mutex> ul(ele_->m);
            std::cout << "handler is called" << std::endl;
            while (ele_->flag) {
                ele_->cv.wait(ul);
                char data[1024];
                int count = ::recv(sock_, data, 1024, ::MSG_DONTWAIT);
                if (count == -1) {
                    std::perror("recv failed");
                    continue;
                } else if (count != 0) {
                    std::cout << std::string(data, count) << std::endl;
                }
            }
        };

        Socket sock;
        std::map<int, element> elements;

#ifdef USE_LINUX
        int epfd;
        epoll_event *evs;
        const size_t MAX_EVENT = 4096;
#endif

        static bool addfd2epoll(int epfd, int fd) {
            ::epoll_event ev;
            ev.events = ::EPOLLIN | ::EPOLLET | ::EPOLLERR;
            ev.data.fd = fd;
            if (::epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
                perror("epoll add failed");
                return false;
            }
            return true;
        }

        static bool delfd2epoll(int epfd, int fd) {
            ::epoll_event ev;
            ev.data.fd = fd;
            if (::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
                perror("epoll del failed");
                return false;
            }
            return true;
        }

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


        virtual ~Server() {
            if (evs != nullptr) {
                delete[] evs;
            }
        }

        virtual void start() {
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

                    //错误
                    if (evs[i].events & ::EPOLLERR) {
                        delfd2epoll(epfd, evs[i].data.fd);
                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            elements.erase(it);
                        }
                    }
                        //新请求
                    else if (evs[i].events & ::EPOLLIN && evs[i].data.fd == sock.get_socket()) {
                        sockaddr_in client;
                        socklen_t len = sizeof(client);
                        int cfd = ::accept(sock.get_socket(), (sockaddr *) (&client), &len);
                        if (cfd == -1) {
                            perror("accept failed");
                            continue;
                        }
                        char addr[30] = {0};
                        inet_ntop(Socket::IPV4, &client, addr, sizeof(client));
                        printf("get connection from: %s\n", addr);

                        if (addfd2epoll(this->epfd, cfd)) {
                            auto[it, flag] = elements.insert(std::pair<int, element>(cfd, element()));
                            if (flag) {
                                it->second.flag = true;
                                it->second.t = std::thread(handler, &it->second, it->first);
                                it->second.t.detach();
                            }
                        }
                    }
                        //可读
                    else if (evs[i].events & ::EPOLLIN) {

                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            it->second.cv.notify_all();
                        }
                    }

                }
                if (con < 0) {
                    break;
                }
            }

        }
    };
}