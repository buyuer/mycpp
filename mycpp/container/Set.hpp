#pragma once

#include <cstddef>

#include "Base.hpp"

#include "Map.hpp"
#include "Vector.hpp"

template <typename KEY, typename COMPARE = LessThan<KEY>>
class Set
{
  public:
    using value_type = KEY;

    using size_type = size_t;

    using iterator = value_type *;

    Set() = default;

    ~Set() = default;

    Set(const Set &other) noexcept
        : data_(const_cast<decltype(data_) &>(other.data_)),
          compare(other.compare)
    {
    }

    Set(Set &&other) noexcept
        : data_(static_cast<decltype(data_) &&>(other.data_)),
          compare(other.compare)
    {
    }

    Set &operator=(const Set &other) noexcept
    {
        this->data_   = const_cast<decltype(data_) &>(other.data_);
        this->compare = other.compare;
        return *this;
    }

    Set &operator=(Set &&other) noexcept
    {
        this->data_   = static_cast<decltype(data_) &&>(other.data_);
        this->compare = other.compare;
        return *this;
    }

    bool empty() const
    {
        return data_.empty();
    }

    size_type size() const
    {
        return data_.size();
    }

    Pair<iterator, bool> insert(const KEY &value)
    {
        auto ret = this->find(value);
        if (ret == this->end())
        {
            for (auto &key : data_)
            {
                if (compare(key, value))
                {
                    return Pair<iterator, bool>{data_.emplace(&key, value),
                                                true};
                }
            }
            return Pair<iterator, bool>{data_.emplace(data_.end(), value),
                                        true};
        }
        return Pair<iterator, bool>{ret, false};
    }

    size_type erase(const KEY &key)
    {
        auto ret = this->find(key);
        if (ret != this->end())
        {
            data_.erase(ret);
            return 1;
        }
        return 0;
    }

    iterator find(const KEY &key)
    {
        for (auto &k : data_)
        {
            if (IS_EQUAL(k, key))
            {
                return &k;
            }
        }

        return data_.end();
    }

    const iterator find(const KEY &key) const
    {
        return find(key);
    }

    iterator begin()
    {
        return data_.begin();
    }

    const iterator begin() const
    {
        return data_.begin();
    }

    iterator end()
    {
        return data_.end();
    }

    const iterator end() const
    {
        return data_.end();
    }

  private:
    Vector<KEY> data_;
    COMPARE     compare;

    bool IS_EQUAL(const KEY &a, const KEY &b)
    {
        return !compare(a, b) && !compare(b, a);
    }
};
