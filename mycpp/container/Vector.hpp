#pragma once

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "Base.hpp"

#define VECTOR_DEFAULT_INCREMENT_COUNT 16

template <typename T> T *memmove_up(T *dst, T *src, size_t size)
{
    for (size_t index = size; index > 0; --index)
    {
        new (dst + index - 1) T(static_cast<T &&>(*(src + index - 1)));
    }
    return dst;
}

template <typename T> T *memmove_down(T *dst, T *src, size_t size)
{
    for (size_t index = 0; index < size; ++index)
    {
        new (dst + index) T(static_cast<T &&>(*(src + index)));
    }
    return dst;
}

template <typename T> T *memmove_(T *dst, T *src, size_t size)
{
    if (dst > src)
    {
        return memmove_up(dst, src, size);
    }
    else if (dst < src)
    {
        return memmove_down(dst, src, size);
    }
    else
    {
        return nullptr;
    }
}

template <typename T> T *memcpy_(T *dst, T *src, size_t size)
{
    for (size_t index = 0; index < size; ++index)
    {
        new (dst + index) T(static_cast<const T &>(*(src + index)));
    }
    return dst;
}

template <size_t S = 1> class AllocatorAll
{
  public:
    void *malloc(size_t size)
    {
        auto alloc = ::malloc(size * S);
        assert(alloc != nullptr);
        return alloc;
    }

    void *realloc(void *ptr, size_t size)
    {
        auto alloc = ::realloc(ptr, size * S);
        assert(alloc != nullptr);
        return alloc;
    }

    void *memcpy(void *dst, void *src, size_t size)
    {
        return ::memcpy(dst, src, size * S);
    }

    void *memmove(void *dst, void *src, size_t size)
    {
        return ::memmove(dst, src, size * S);
    }

    void *memset(void *src, int c, size_t n)
    {
        return ::memset(src, c, n);
    }

    void free(void *ptr)
    {
        ::free(ptr);
    }
};

template <typename T> class AllocatorOne
{
  public:
    void *malloc(size_t size)
    {
        size_      = size;
        auto alloc = ::malloc(size);
        assert(alloc != nullptr);
        return alloc;
    }

    void *realloc(void *ptr, size_t size)
    {
        if (size <= size_)
        {
            return ptr;
        }

        void *alloc = ::malloc(size);
        assert(alloc != nullptr);
        ::memmove_<T>((T *)alloc, (T *)ptr, size_ / sizeof(T));
        ::free(ptr);
        size_ = size;

        return alloc;
    }

    void *memcpy(void *dst, void *src, size_t size)
    {
        return ::memcpy_<T>((T *)dst, (T *)src, size / sizeof(T));
    }

    void *memmove(void *dst, void *src, size_t size)
    {
        return ::memmove_<T>((T *)dst, (T *)src, size / sizeof(T));
    }

    void *memset(void *src, int c, size_t n)
    {
        return ::memset(src, c, n);
    }

    void free(void *ptr)
    {
        ::free(ptr);
    }

  private:
    size_t size_{0};
};

template <typename T, typename ALLOCATOR = AllocatorOne<T>> class Vector
{
  public:
    using size_type = size_t;

    using iterator = T *;

    /*class iterator {
    public:
        iterator(T *pos) : pos_(pos) {}

        bool operator!=(const iterator &other) const {
            return other.pos_ != this->pos_;
        }

        bool operator!=(const T *other) const {
            return other != this->pos_;
        }

        bool operator==(const iterator &other) const {
            return other.pos_ == this->pos_;
        }

        bool operator==(const T *other) const {
            return other == this->pos_;
        }

        bool operator<(const iterator &other) const {
            return other.pos_ < this->pos_;
        }

        bool operator<(const T *other) const {
            return other < this->pos_;
        }

        bool operator<=(const iterator &other) const {
            return other.pos_ <= this->pos_;
        }

        bool operator<=(const T *other) const {
            return other <= this->pos_;
        }

        bool operator>(const iterator &other) const {
            return other.pos_ > this->pos_;
        }

        bool operator>(const T *other) const {
            return other > this->pos_;
        }

        bool operator>=(const iterator &other) const {
            return other.pos_ >= this->pos_;
        }

        bool operator>=(const T *other) const {
            return other >= this->pos_;
        }

        size_type operator-(const iterator &other) const {
            return this->pos_ - other.pos_;
        }

        iterator &operator+(const size_type opera) {
            this->pos_ += opera;
            return *this;
        }

        iterator &operator+(const int opera) {
            return this->operator+((size_type) opera);
        }

        iterator &operator-(const size_type opera) {
            this->pos_ -= opera;
            return *this;
        }

        iterator &operator-(const int opera) {
            return this->operator-((size_type) opera);
        }

        iterator &operator++() {
            ++pos_;
            return *this;
        }

        iterator &operator--() {
            --pos_;
            return *this;
        }

        iterator operator++(int) {
            auto ret = *this;
            pos_++;
            return ret;
        }

        iterator operator--(int) {
            auto ret = *this;
            pos_--;
            return ret;
        }

        T &operator*() const {
            return *pos_;
        }

        operator T *() {
            return pos_;
        }

        operator const T *() {
            return pos_;
        }

        operator void *() {
            return pos_;
        }

        friend Vector;

    private:
        T *pos_{nullptr};

    };*/

    explicit Vector()
    {
        init_(VECTOR_DEFAULT_INCREMENT_COUNT, 0);
    }

    explicit Vector(size_type size)
    {
        init_(size + VECTOR_DEFAULT_INCREMENT_COUNT, size);
    }

    explicit Vector(size_type size, const T &value)
    {
        init_(size + VECTOR_DEFAULT_INCREMENT_COUNT, size, value);
    }

    explicit Vector(iterator first, iterator last)
    {
        init_(last - first + VECTOR_DEFAULT_INCREMENT_COUNT, 0);
        this->insert(data_, first, last);
    }

    ~Vector()
    {
        clear();
        allocator.free(data_);
    }

    Vector(const Vector &other)
    {
        copy_(other);
    }

    Vector(Vector &&other) noexcept
    {
        move_(static_cast<Vector &&>(other));
    }

    Vector &operator=(const Vector &other)
    {
        return copy_(other);
    };

    Vector &operator=(Vector &&other) noexcept
    {
        return move_(static_cast<Vector &&>(other));
    }

    T &at(size_type pos)
    {
        if (pos >= size_)
        {
            pos = size_ - 1;
        }
        return operator[](pos);
    }

    const T &at(size_type pos) const
    {
        return operator[](pos);
    }

    T &operator[](size_type pos)
    {
        return *(data_ + pos);
    }

    const T &operator[](size_type pos) const
    {
        return *(data_ + pos);
    }

    T &front()
    {
        return *data_;
    }

    T &back()
    {
        return *(data_ + size_ - 1);
    }

    T *data()
    {
        return data_;
    }

    bool empty() const noexcept
    {
        return size_ == 0;
    }

    size_type size() const noexcept
    {
        return size_;
    }

    void reserve(size_type cap)
    {
        if (cap > cap_)
        {
            cap_  = cap;
            data_ = (T *)allocator.realloc(data_, SIZE_BYTE(cap_));
            if (size_ > cap_)
            {
                size_ = cap_;
            }
        }
    }

    size_type capacity() const noexcept
    {
        return cap_;
    }

    void clear() noexcept
    {
        for (size_type index = 0; index < size_; ++index)
        {
            (data_ + index)->~T();
        }
        allocator.memset(data_, 0, cap_ * sizeof(T));
        size_ = 0;
    }

    iterator insert(iterator pos, const T &value)
    {
        return emplace(pos, value);
    }

    iterator insert(iterator pos, const iterator first, const iterator last)
    {
        auto      new_data_size  = last - first;
        auto      move_data_size = this->end() - pos;
        size_type offset         = pos - data_;

        size_ += new_data_size;
        if (size_ >= cap_)
        {
            this->reserve(size_ + VECTOR_DEFAULT_INCREMENT_COUNT);
        }
        iterator new_pos = data_ + offset;

        auto *move_data_pos = new_pos + new_data_size;
        allocator.memmove(move_data_pos, new_pos, SIZE_BYTE(move_data_size));
        allocator.memcpy(new_pos, first, SIZE_BYTE(new_data_size));
        return new_pos;
    }

    template <typename... ARGS>
    iterator emplace(const iterator pos, ARGS &&...args)
    {
        size_type offset = pos - data_;
        if (size_ >= cap_)
        {
            this->reserve(size_ + VECTOR_DEFAULT_INCREMENT_COUNT);
        }
        iterator new_pos = data_ + offset;

        allocator.memmove(new_pos + 1, new_pos, SIZE_BYTE(size_ - offset));
        new (new_pos) T(args...);
        ++size_;
        return new_pos;
    }

    iterator erase(const iterator pos)
    {
        return erase(pos, pos + 1);
    }

    iterator erase(const iterator first, const iterator last)
    {
        for (iterator index = first; index < last; ++index)
        {
            index->~T();
        }
        auto erase_size = last - first;
        auto move_size  = this->end() - last;
        allocator.memmove(first, last, SIZE_BYTE(move_size));
        size_ -= erase_size;
        return first;
    }

    void push_back(const T v)
    {
        emplace_back(v);
    }

    void pop_back()
    {
        if (size_)
        {
            --size_;
            (data_ + size_)->~T();
        }
    }

    template <typename... ARGS> T &emplace_back(ARGS &&...args)
    {
        return *emplace(this->end(), args...);
    }

    void resize(size_type size)
    {
        size_ = size;
        if (size_ >= cap_)
        {
            this->reserve(size_ + VECTOR_DEFAULT_INCREMENT_COUNT);
        }
    }

    void swap(Vector &other) noexcept
    {
        auto temp_cap  = cap_;
        auto temp_size = size_;
        auto temp_data = data_;
        cap_           = other.cap_;
        size_          = other.size_;
        data_          = other.data_;
        other.cap_     = temp_cap;
        other.size_    = temp_size;
        other.data_    = temp_data;
    }

    iterator begin() noexcept
    {
        return iterator(data_);
    }

    const iterator begin() const noexcept
    {
        return iterator(data_);
    }

    iterator end() noexcept
    {
        return iterator(data_ + size_);
    }

    const iterator end() const
    {
        return iterator(data_ + size_);
    }

  private:
    inline static size_t SIZE_BYTE(size_t size)
    {
        return size * sizeof(T);
    }

    ALLOCATOR allocator;

    template <typename... ARGS>
    void init_(size_type cap, size_type size, ARGS... args)
    {
        size_ = size;
        cap_  = cap;
        data_ = (T *)allocator.malloc(SIZE_BYTE(cap_));

        for (size_type index = 0; index < size_; ++index)
        {
            new (data_ + index) T(args...);
        }
    }

    Vector &copy_(const Vector &other)
    {
        cap_  = other.cap_;
        size_ = other.size_;
        data_ = (T *)allocator.malloc(SIZE_BYTE(cap_));
        allocator.memcpy(data_, other.data_, SIZE_BYTE(size_));
        return *this;
    }

    Vector &move_(Vector &&other)
    {
        cap_  = other.cap_;
        size_ = other.size_;
        data_ = other.data_;

        other.cap_  = 0;
        other.size_ = 0;
        other.data_ = nullptr;

        return *this;
    }

    size_type cap_{0};
    size_type size_{0};
    T        *data_{nullptr};
};