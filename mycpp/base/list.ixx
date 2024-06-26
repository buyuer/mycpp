module;
#include <cstddef>
export module mycpp.base:list;

export namespace mycpp
{

template <class T>
class list
{
  public:
    list() = default;

    virtual ~list() = default;

    virtual void add(const T &e) = 0;

    virtual void add(const size_t &pos, const T &e) = 0;

    virtual void remove() = 0;

    virtual void remove(const size_t &pos) = 0;

    virtual size_t get_size() const = 0;

    virtual T &get(size_t pos) const = 0;

    virtual T &operator[](size_t pos) const = 0;

    virtual void clean() = 0;

    virtual T &get_front() = 0;

    virtual T &get_back() = 0;

    virtual bool is_empty() = 0;
};

} // namespace mycpp