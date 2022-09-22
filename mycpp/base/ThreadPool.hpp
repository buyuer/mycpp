#pragma once

#include "mycpp/base/interface.hpp"

namespace mycpp
{

class ThreadPool
{
  private:
    using ThreadID = std::thread::id;

    struct Runner
    {
        using Entry = void (*)(void *);

        Entry entry;
        void *arg;
    };

    enum Flag : uint32_t
    {
        FLAG_NONE = 0,
        FLAG_EXIT = 0x01,
    };

    std::mutex                mu_runners;
    std::mutex                mu_cv;
    std::condition_variable   cv;
    std::queue<Runner>        runners;
    std::vector<std::jthread> thread_pool;
    uint32_t                  flags{};

    static void thread_entry(ThreadPool *own)
    {
        while (true)
        {
            if (own->flags bitand FLAG_EXIT)
            {
                return;
            }

            std::unique_lock<std::mutex> ul(own->mu_runners);
            if (own->runners.empty())
            {
                std::unique_lock<std::mutex> ul_cv(own->mu_cv);
                ul.unlock();
                own->cv.wait(ul_cv);
            }
            else
            {
                auto runner{own->runners.back()};
                own->runners.pop();
                ul.unlock();
                runner.entry(runner.arg);
            }
        }
    }

    void destroy()
    {
        for (auto &t : thread_pool)
        {
            auto stop  = t.get_stop_source();
            auto token = t.get_stop_token();

            std::cout << t.request_stop() << std::endl;

            flags |= FLAG_EXIT;

            cv.notify_all();
        }
    }

  public:
    explicit ThreadPool(uint32_t count = std::thread::hardware_concurrency())
    {
        thread_pool.reserve(count);

        for (uint32_t i = 0; i < count; i++)
        {
            auto &t =
                thread_pool.emplace_back(std::jthread(thread_entry, this));
            t.detach();
        }
    }

    ~ThreadPool()
    {
        destroy();
    };

    ThreadPool(const ThreadPool &)            = delete;
    ThreadPool(ThreadPool &&)                 = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&)      = delete;

    void run(Runner::Entry entry, void *arg)
    {
        std::unique_lock<std::mutex> ul(this->mu_runners);
        runners.emplace(ThreadPool::Runner{
            .entry = entry,
            .arg   = arg,
        });
        ul.unlock();
        cv.notify_one();
    }
};
} // namespace mycpp