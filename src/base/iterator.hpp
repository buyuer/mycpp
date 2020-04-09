#pragma once

namespace my {

    template<class T>
    class iterator {
    public:
        virtual T &operator*() = 0;

        virtual T *operator->() = 0;

        virtual my::iterator<T> &operator++() = 0;

        virtual my::iterator<T> &operator--() = 0;

        virtual my::iterator<T> &operator++(int) = 0;

        virtual my::iterator<T> &operator--(int) = 0;

        virtual bool operator!=(const my::iterator<T> &) noexcept(false) = 0;

        virtual bool operator==(const my::iterator<T> &) noexcept(false) = 0;
    };
}