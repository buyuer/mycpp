module;
export module mycpp.base:iterator;

export namespace mycpp
{

template <class T>
class iterator
{
  public:
    virtual T &operator*() = 0;

    virtual T *operator->() = 0;

    virtual mycpp::iterator<T> &operator++() = 0;

    virtual mycpp::iterator<T> &operator--() = 0;

    virtual mycpp::iterator<T> &operator++(int) = 0;

    virtual mycpp::iterator<T> &operator--(int) = 0;

    virtual bool operator!=(const mycpp::iterator<T> &) noexcept(false) = 0;

    virtual bool operator==(const mycpp::iterator<T> &) noexcept(false) = 0;
};
} // namespace mycpp