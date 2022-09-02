#pragma once

#include "base/interface.hpp"

namespace my {

    class ThreadPool {

    private:

        using ThreadID = std::thread::id;
        using Worker = void (*)(void *);

        struct element {
            Worker f;
            void *arg;
        };

        enum Flag : uint32_t {
            FLAG_NONE = 0,
            FLAG_EXIT = 0x01,
        };

        std::mutex m_tasks;
        std::mutex m_cv;
        std::condition_variable cv;
        std::queue<element> tasks;
        std::vector<std::jthread> pool;
        uint32_t flags{};


        static void thread_entry(ThreadPool *own) {

            while (true) {
                if (own->flags bitand FLAG_EXIT) {
                    return;
                }

                std::unique_lock<std::mutex> ul(own->m_tasks);
                if (own->tasks.empty()) {
                    std::unique_lock<std::mutex> ul_cv(own->m_cv);
                    ul.unlock();
                    own->cv.wait(ul_cv);
                } else {
                    auto temp = own->tasks.back();
                    Worker now = temp.f;
                    own->tasks.pop();
                    ul.unlock();
                    now(temp.arg);
                }
            }

        }

        void destroy() {
            for (auto &t: pool) {
                auto stop = t.get_stop_source();
                auto token = t.get_stop_token();

                std::cout << t.request_stop() << std::endl;

                flags |= FLAG_EXIT;

                cv.notify_all();
            }
        }

    public:
        explicit ThreadPool(uint32_t count = std::thread::hardware_concurrency()) {
            pool.reserve(count);

            for (uint32_t i = 0; i < count; i++) {
                auto &t = pool.emplace_back(std::jthread(thread_entry, this));
                t.detach();
            }
        }

        ~ThreadPool() {
            destroy();
        };

        void run(Worker worker, void *arg) {
            std::unique_lock<std::mutex> ul(this->m_tasks);
            tasks.push({.f = worker, .arg = arg});
            ul.unlock();
            cv.notify_one();
        }
    };
}