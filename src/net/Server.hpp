#pragma once

#include "net/Socket.hpp"

namespace my {

    class Server {

    private:

        struct element {
            std::mutex m;
            std::condition_variable cv;
            std::thread t;
            bool run;

            element() : run(false) {}

            element(const element &h) {
                this->run = h.run;
            }

            element &operator=(const element &h) {
                this->run = h.run;
                return *this;
            }
        };

        using func = void (*)(std::map<int, element> *, const int);

        func handler = [](std::map<int, element> *es_, const int sock_) -> void {

            std::cout << "handler is called" << std::endl;

            while (true) {

                auto it = es_->find(sock_);

                if (it == es_->end()) {
                    it->second.run = false;
                    break;
                }

                it->second.run = true;

                std::unique_lock<std::mutex> ul(it->second.m);
                it->second.cv.wait(ul);

                char data[1024];
                int count = ::recv(sock_, data, 1024, ::MSG_DONTWAIT);
                if (count == -1) {
                    std::perror("recv failed");
                    it->second.run = false;
                    break;
                } else if (count != 0) {
                    std::cout << std::string(data, count) << std::endl;
                }

                it->second.run = false;
            }

            return;
        };

        Socket sock;
        std::map<int, element> elements;

#ifdef USE_LINUX
        int epfd;
        epoll_event *evs;
        const size_t MAX_EVENT = 4096;

        static bool addfd2epoll(int epfd, int fd) {
            ::epoll_event ev;
            ev.events = ::EPOLLIN | ::EPOLLET | ::EPOLLERR | ::EPOLLRDHUP | ::EPOLLHUP | ::EPOLLOUT;
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


        virtual ~Server() {
            if (evs != nullptr) {
                delete[] evs;
            }
            for(auto &i : elements){
                ::close(i.first);
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

                    if (evs[i].events & ::EPOLLERR) {
                        delfd2epoll(epfd, evs[i].data.fd);
                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            it->second.cv.notify_all();
                            elements.erase(it);
                        }
                        if (evs[i].data.fd == sock.get_socket()) {
                            perror("server socket err");
                            break;
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
                                it->second.t = std::thread(handler, &elements, it->first);
                                it->second.t.detach();
                            }
                        }
                    }
                        //可读
                    else if (!(evs[i].events & ~(::EPOLLIN | ::EPOLLOUT))) {

                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            it->second.cv.notify_all();
                        }

                    }
                        //远程客户端关闭
                    else if (evs[i].events & (::EPOLLRDHUP | ::EPOLLIN)) {

                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            it->second.cv.notify_all();
                            elements.erase(it);
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