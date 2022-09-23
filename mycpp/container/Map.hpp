#pragma once

#include <cstddef>

#include "Vector.hpp"

#include "Base.hpp"

template <typename FIRST, typename SECOND>
struct Pair
{
    FIRST  first;
    SECOND second;
};

template <typename T>
class LessThan
{
  public:
    bool operator()(const T &a, const T &b)
    {
        return a < b;
    }
};

template <typename T>
class MoreThan
{
  public:
    bool operator()(const T &a, const T &b)
    {
        return a > b;
    }
};

template <typename KEY, typename VALUE, typename COMPARE = LessThan<KEY>>
class Map
{
  private:
    using node_type = Pair<KEY, VALUE>;

  public:
    using iterator = node_type *;

    using size_type = size_t;

    explicit Map() = default;

    ~Map() = default;

    Map(const Map &other) noexcept
        : data_(const_cast<decltype(data_) &>(other.data_)),
          compare(other.compare)
    {
    }

    Map(Map &&other) noexcept
        : data_(static_cast<decltype(data_) &&>(other.data_)),
          compare(other.compare)
    {
    }

    Map &operator=(const Map &other) noexcept
    {
        this->data_   = const_cast<decltype(data_) &>(other.data_);
        this->compare = other.compare;
        return *this;
    }

    Map &operator=(Map &&other) noexcept
    {
        this->data_   = static_cast<decltype(data_) &&>(other.data_);
        this->compare = other.compare;
        return *this;
    }

    VALUE &operator[](const KEY &key)
    {
        auto ret = find(key);
        if (ret != end())
        {
            return ret->second;
        }

        for (auto &pair : data_)
        {
            if (compare(key, pair.first))
            {
                return data_
                    .insert(&pair, node_type{.first = key, .second = {}})
                    ->second;
            }
        }

        return data_.emplace_back(node_type{.first = key}).second;
    }

    VALUE &operator[](KEY &&key)
    {
        return operator[](static_cast<const KEY &>(key));
    }

    bool empty() const noexcept
    {
        return data_.empty();
    }

    size_type size() const noexcept
    {
        return data_.size();
    }

    void clear() noexcept
    {
        data_.clear();
    }

    template <typename... ARGS>
    Pair<iterator, bool> try_emplace(const KEY &key, ARGS... args)
    {
        auto ret = this->find(key);
        if (ret != this->end())
        {
            return Pair<iterator, bool>{ret, false};
        }
        else
        {
            for (auto &pair : data_)
            {
                if (compare(key, pair.first))
                {
                    return Pair<iterator, bool>{
                        data_.emplace(&pair, node_type{key, args...}), true};
                }
            }

            return Pair<iterator, bool>{
                data_.emplace(data_.end(), node_type{key, args...}), true};
        }
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
        for (auto &pair : data_)
        {
            if (is_equal(key, pair.first))
            {
                return &pair;
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
    Vector<node_type> data_;
    COMPARE           compare;

    bool is_equal(const KEY &a, const KEY &b)
    {
        return !compare(a, b) && !compare(b, a);
    }
};