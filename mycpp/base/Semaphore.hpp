#pragma once

#include "base/interface.hpp"

namespace my
{

class Semaphore
{

  private:
    s64                     count;
    std::mutex              m;
    std::condition_variable cv;

  public:
    Semaphore(s64 count_) : count(count_)
    {
    }

    ~Semaphore()
    {
    }

    void release(s64 update = 0)
    {
    }

    void acquire()
    {
    }
};

} // namespace my