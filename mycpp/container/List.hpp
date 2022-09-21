#pragma once

#include <cstddef>

#include "Base.hpp"

template <typename T> class List
{
  private:
    struct Context
    {
        T        target;
        Context *next{nullptr};
        Context *last{nullptr};

      public:
        template <typename... ARGS>
        explicit Context(ARGS &&...args) : target(args...)
        {
        }
    };

  public:
    using size_type = size_t;

    class iterator
    {
        Context *node;

      public:
        explicit iterator(Context *nd) : node(nd)
        {
        }

        bool operator!=(const iterator &it)
        {
            return node != it.node;
        }

        iterator &operator++()
        {
            node = node->next;
            return *this;
        }

        T &operator*()
        {
            return node->target;
        }

        friend List;
    };

    List() = default;

    List(const List<T> &) = delete;

    List(List<T> &&) = delete;

    List<T> &operator=(const List<T> &) = delete;

    List<T> &operator=(List<T> &&) = delete;

    ~List()
    {
        clear();
    }

    T &front()
    {
        return head_->target;
    }

    const T &front() const
    {
        return front();
    }

    T &back()
    {
        return tail_->target;
    }

    const T &back() const
    {
        return back();
    }

    bool empty() const
    {
        return size_ == 0;
    }

    size_type size()
    {
        return size_;
    }

    void clear()
    {
        auto cur = head_;
        while (cur != nullptr)
        {
            auto temp = cur;
            cur       = cur->next;
            delete temp;
        }
        head_ = nullptr;
        tail_ = nullptr;
    }

    void push_back(const T &target)
    {
        emplace_back(target);
    }

    template <class... Args> iterator emplace_back(Args &&...args)
    {
        auto new_node  = new Context(args...);
        new_node->last = tail_;
        if (tail_ != nullptr)
        {
            tail_->next = new_node;
        }
        tail_ = new_node;
        if (head_ == nullptr)
        {
            head_ = tail_;
        }
        ++size_;

        return iterator(tail_);
    }

    void pop_back()
    {
        if (tail_ == nullptr)
        {
            return;
        }
        auto temp = tail_;
        tail_     = tail_->last;
        if (tail_ != nullptr)
        {
            tail_->next = nullptr;
        }
        delete temp;
        --size_;
        if (size_ == 0)
        {
            head_ = nullptr;
        }
    }

    void pop_front()
    {
        if (head_ == nullptr)
        {
            return;
        }
        auto temp = head_;
        head_     = head_->next;
        if (head_ != nullptr)
        {
            head_->last = nullptr;
        }
        delete temp;
        --size_;
        if (size_ == 0)
        {
            tail_ = nullptr;
        }
    }

    /* Non-C ++ standard interface
    bool remove(size_type index)
    {
        if (index >= size_) {
            return false;
        }
        Context** indirect = &head_;
        for (size_t i = 0; i < index; i++) {
            indirect = &(*indirect)->next;
        }
        Context* temp = *indirect;
        if (temp == tail_ && size_ > 1) {
            tail_ = (Context*)(((uint8_t*)(indirect))
                               - (offsetof(Context, next)));
        } else if (1 == size_) {
            tail_ = nullptr;
        }
        *indirect = (*indirect)->next;
        delete temp;
        --size_;
        return true;
    }

    T& get(size_type index)
    {
        if (index >= size_) {
            index = size_ - 1;
        }
        return operator[](index);
    }

    T& operator[](size_type index)
    {
        Context* node = head_;
        for (size_t i = 0; i < index; i++) {
            node = node->next;
        }
        return node->target;
    }*/

    List::iterator begin()
    {
        return iterator(head_);
    }

    List::iterator end()
    {
        return iterator(nullptr);
    }

  private:
    Context  *head_{nullptr};
    Context  *tail_{nullptr};
    size_type size_{0};
};
