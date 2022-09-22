#pragma once

namespace mycpp
{

template <class T> class stack
{
  public:
    stack()
    {
    }

    virtual ~stack()
    {
    }

    virtual void push(const T &) = 0;

    virtual T pop() = 0;

    virtual T &peek() = 0;
};
} // namespace mycpp