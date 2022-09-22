#pragma once

#include "mycpp/base/interface.hpp"

namespace mycpp
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

} // namespace mycpp