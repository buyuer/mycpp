#pragma once

#include "base/interface.hpp"

namespace my{

    class semaphore{

    private:
        s64 count;
        std::mutex m;
        std::condition_variable cv;


    public:

        semaphore(s64 count_):count(count_){

        }

        ~semaphore(){

        }

        void release(s64 update = 0){

        }

        void acquire(){

        }

    };

}