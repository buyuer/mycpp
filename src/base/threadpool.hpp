#pragma once

#include "base/interface.hpp"

namespace my{

    template<class worker>
    class threadpool{

    private:
        std::mutex m;
        std::condition_variable cv;
        std::queue<worker> tasks;

        static void func(threadpool<worker>& own){

            while (true){

                std::unique_lock<std::mutex> ul(own.m);
                if(own.tasks.empty()){
                    ul.unlock();
                    own.cv.wait(ul);
                }
                else{

                }

            }

        }

    public:
        threadpool(s32 count){
            std::thread
        }
    };

}