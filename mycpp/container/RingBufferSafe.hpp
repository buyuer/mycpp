#pragma once

#include <cstddef>
#include <mutex>

template <typename T_>
class RingBufferSafe
{
  public:
    using size_type = std::size_t;

    RingBufferSafe() = default;

    explicit RingBufferSafe(size_type cap)
    {
        recap(cap);
    }

    ~RingBufferSafe()
    {
        delete _data;
    }

    void recap(size_type cap)
    {
        _cap = cap;
        delete _data;
        _data = new T_[_cap];
        _front = 0;
        _back = 0;
    }

    size_type push_back_force(T_ *data, size_type num)
    {
        std::unique_lock<std::mutex> ul(_mu);
        num = num > _cap ? _cap : num;

        for (size_type index = 0; index < num; ++index)
        {
            auto pos = _back++ % _cap;
            *(_data + pos) = *(data + index);
        }

        auto size = _back - _front;
        _front = size > _cap ? _back - _cap : _front;

        return num;
    }

    size_type push_back(T_ *data, size_type num)
    {
        std::unique_lock<std::mutex> ul(_mu);
        auto size = _back - _front;
        auto idle = _cap - size;
        num = num > idle ? idle : num;

        for (size_type index = 0; index < num; ++index)
        {
            auto pos = _back++ % _cap;
            *(_data + pos) = *(data + index);
        }

        return num;
    }

    size_type pop_front(T_ *data, size_type num)
    {
        std::unique_lock<std::mutex> ul(_mu);
        auto size = _back - _front;
        num = num > size ? size : num;

        for (size_type index = 0; index < num; ++index)
        {
            auto pos = _front++ % _cap;
            *(data + index) = *(_data + pos);
        }

        return num;
    }

    size_type get_size()
    {
        return _back - _front;
    }

    size_type get_cap()
    {
        return _cap;
    }

  private:
    T_ *_data{};
    size_type _back{};
    size_type _front{};
    size_type _cap{};
    std::mutex _mu;
};