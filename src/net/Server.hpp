#pragma once

#include "net/Socket.hpp"

namespace my {

    class Server {

    public:

        class sock_info {
            int sockfd;
        public:
            std::string ip;
            int port;

            void close() const {
                ::close(sockfd);
            }

            int get_sockfd() const {
                return this->sockfd;
            }

            sock_info(int s_) : sockfd(s_) {}
        };

        using user_handler = void (*)(std::iostream &, const sock_info &);

        user_handler handler;

    protected:

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

        using func = void (*)(std::map<int, element> *, const sock_info, const user_handler);

        func handler_thread = [](std::map<int, element> *es_, const sock_info sockInfo,
                                 const user_handler handler_) -> void {

            while (true) {

                auto it = es_->find(sockInfo.get_sockfd());

                if (it == es_->end()) {
                    it->second.run = false;
                    break;
                }

                it->second.run = true;

                std::unique_lock<std::mutex> ul(it->second.m);
                it->second.cv.wait(ul);

                my::socketbuff buff(sockInfo.get_sockfd(), sockInfo.get_sockfd());
                std::iostream io(&buff);

                handler_(io, sockInfo);

                it->second.run = false;
            }
            end:
            return;
        };

        Socket listen_socket;
        std::map<int, element> elements;

        int epfd;
        ::epoll_event *evs;
        const size_t MAX_EVENT = 4096;

        static bool addfd2epoll(int epfd, int fd) {
            ::epoll_event ev;
            //监听事件：读、写、错误、边缘触发模式（ET）、关闭事件
            ev.events = ::EPOLLIN | ::EPOLLERR | ::EPOLLRDHUP | ::EPOLLHUP | ::EPOLLOUT;
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
                listen_socket(Socket::IPV4), epfd(-1), evs(nullptr), handler(nullptr) {
            listen_socket.bind("127.0.0.1", port);
            if ((epfd = ::epoll_create(MAX_EVENT)) == -1) {
                perror("epoll_create failed");
                exit(0);
            }
            evs = new epoll_event[MAX_EVENT];

            handler = [](std::iostream &io, const sock_info &sockInfo) -> void {
                std::string str("my Server");
                std::cout << str << std::endl;
                io << str << std::flush;
                sockInfo.close();
            };
        }


        virtual ~Server() {
            if (evs != nullptr) {
                delete[] evs;
            }
            for (auto &i : elements) {
                ::close(i.first);
            }
        }

        virtual void start() {
            listen_socket.listen();

            epoll_event ev;
            ev.events = ::EPOLLIN | ::EPOLLET | ::EPOLLERR;
            ev.data.fd = listen_socket.get_socket();

            if (::epoll_ctl(epfd, EPOLL_CTL_ADD, ev.data.fd, &ev) == -1) {
                perror("epoll_ctl failed");
                exit(0);
            }

            while (true) {

                int con = ::epoll_wait(epfd, evs, MAX_EVENT, -1);

                for (int i = 0; i < con; i++) {

                    //错误处理
                    if (evs[i].events & ::EPOLLERR) {
                        delfd2epoll(epfd, evs[i].data.fd);
                        auto it = elements.find(evs[i].data.fd);
                        if (it != elements.end()) {
                            it->second.cv.notify_all();
                            elements.erase(it);
                        }
                        if (evs[i].data.fd == listen_socket.get_socket()) {
                            perror("server socket err");
                            break;
                        }
                    }
                        //新请求
                    else if (evs[i].events & ::EPOLLIN && evs[i].data.fd == listen_socket.get_socket()) {
                        sockaddr_in client;
                        socklen_t len = sizeof(client);
                        int cfd = ::accept(listen_socket.get_socket(), (sockaddr *) (&client), &len);
                        if (cfd == -1) {
                            perror("accept failed");
                            continue;
                        }
                        char addr[30] = {0};
                        inet_ntop(Socket::IPV4, &client, addr, sizeof(client));

                        if (addfd2epoll(this->epfd, cfd)) {
                            auto[it, flag] = elements.insert(std::pair<int, element>(cfd, element()));
                            if (flag) {
                                sock_info sockInfo(cfd);
                                std::string ip(addr, std::strlen(addr));
                                sockInfo.ip = std::move(ip);
                                sockInfo.port = client.sin_port;
                                it->second.t = std::thread(handler_thread, &elements, sockInfo, handler);
                                it->second.t.detach();
                            }
                        }
                    }
                        //可读写（只监听读和写）
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