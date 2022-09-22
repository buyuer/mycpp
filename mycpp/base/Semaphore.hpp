#pragma once

#include "mycpp/base/interface.hpp"

namespace mycpp
{

class Semaphore
{

  private:
    uint64_t                count;
    std::mutex              m;
    std::condition_variable cv;

  public:
    Semaphore(uint64_t count_) : count(count_)
    {
    }

    ~Semaphore()
    {
    }

    void release(uint64_t update = 0)
    {
    }

    void acquire()
    {
    }
};

} // namespace mycpp