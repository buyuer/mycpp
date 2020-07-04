#pragma once

#include "base/interface.hpp"

namespace my{

    class ThreadPool{

    public:
        using Worker = void (*)(void);

    private:

        struct element{
            Worker f;
        };

        std::mutex m_tasks;
        std::mutex m_cv;
        std::condition_variable cv;
        std::queue<element> tasks;

        static void threadf(ThreadPool* own){

            while (true){

                std::unique_lock<std::mutex> ul(own->m_tasks);

                if(own->tasks.empty()){
                    std::unique_lock<std::mutex> ul_cv(own->m_cv);
                    ul.unlock();
                    own->cv.wait(ul_cv);
                }
                else{
                    auto temp = own->tasks.back();
                    Worker now = temp.f;
                    own->tasks.pop();
                    ul.unlock();
                    now();
                }

            }

        }

    public:
        ThreadPool(s32 count){
            for(auto i = 0; i < count; i++){
                std::thread t(my::ThreadPool::threadf, this);
                t.detach();
            }
        }

        void run(my::ThreadPool::Worker f_){
            element e{f_};
            std::unique_lock<std::mutex> ul(this->m_tasks);
            this->tasks.push(e);
            ul.unlock();
            this->cv.notify_one();
        }

    };

}