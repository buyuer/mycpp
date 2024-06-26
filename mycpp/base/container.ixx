module;
#include <cstddef>
export module mycpp.base:container;

import :list;
import :stack;
import :iterator;

export namespace mycpp
{

template <class T>
class ListLink : public list<T>, public stack<T>
{

    struct element
    {
        T        value;
        element *last;
        element *next;

        explicit element(const T &d) : last(nullptr), next(nullptr), value(d)
        {
        }
    };

    size_t   size;
    element *head;
    element *tail;

  public:
    class iterator : public mycpp::iterator<T>
    {

        element *rec;

        explicit iterator(element *p) : rec(p)
        {
        }

      public:
        friend mycpp::ListLink<T>;

        iterator() : rec(nullptr)
        {
        }

        mycpp::ListLink<T>::iterator &operator++() override
        {
            rec = rec->next;
            return *this;
        }

        mycpp::ListLink<T>::iterator &operator--() override
        {
            rec = rec->last;
            return *this;
        }

        mycpp::ListLink<T>::iterator operator++(int i) override
        {
            rec = rec->next;
            return *this;
        }

        mycpp::ListLink<T>::iterator &operator--(int i) override
        {
            rec = rec->last;
            return *this;
        }

        T &operator*() override
        {
            return rec->value;
        }

        T *operator->() override
        {
            return &(rec->value);
        }

        bool operator!=(const mycpp::iterator<T> &it) noexcept(false) override
        {
            const auto i =
                dynamic_cast<const mycpp::ListLink<T>::iterator &>(it);
            return i.rec != this->rec;
        }

        bool operator==(const mycpp::iterator<T> &it) override
        {
            const auto i =
                dynamic_cast<const mycpp::ListLink<T>::iterator &>(it);
            return i.rec == this->rec;
        }

        /*bool operator!=(const mycpp::ListLink<T>::iterator &b) const {
            return b.rec != this->rec;
        }*/

        /*bool operator==(const mycpp::ListLink<T>::iterator &b) const {
            return b.rec == this->rec;
        }*/
    };

    ListLink() : size(0), head(nullptr), tail(nullptr)
    {
    }

    ListLink(const ListLink<T> &ll) : ListLink()
    {

        size               = ll.size;
        element **indirect = &head;
        element  *index    = ll.head;
        element  *last     = nullptr;
        for (size_t i = 0; i < size; i++)
        {
            *indirect         = new element(index->value);
            (*indirect)->last = last;
            last              = *indirect;
            indirect          = &(*indirect)->next;
            index             = index->next;
        }
        tail        = last;
        (*indirect) = head;
        head->last  = last;
    }

    ListLink(ListLink<T> &&ll) noexcept
    {
        size    = ll.size;
        head    = ll.head;
        tail    = ll.tail;
        ll.size = 0;
        ll.head = nullptr;
        ll.tail = nullptr;
    }

    ListLink &operator=(const ListLink<T> &ll) noexcept
    {
        clean();
        new (this) ListLink(ll);
        return *this;
    }

    ListLink &operator=(ListLink<T> &&ll)
    {
        clean();
        new (this) ListLink(static_cast<ListLink<T> &&>(ll));
        return *this;
    }

    ~ListLink()
    {
        if (size == 0 || head == nullptr)
        {
            return;
        }
        else
        {
            clean();
        }
    };

    void clean() override
    {
        element *index = head;
        for (size_t i = 0; i < size; i++)
        {
            element *temp = index;
            index         = index->next;
            delete temp;
        }
        size = 0;
        head = nullptr;
        tail = nullptr;
    }

    void add(const T &e) override
    {
        if (head == nullptr)
        {
            head       = new element(e);
            head->next = head;
            head->last = head;
            tail       = head;
        }
        else
        {
            auto *temp = new element(e);
            temp->last = tail;
            temp->next = nullptr;
            tail->next = temp;
            tail       = temp;
            head->last = nullptr;
        }
        ++size;
    }

    void add(const size_t &pos, const T &e) override
    {
        if (size == 0 || pos >= size - 1)
        {
            add(e);
        }
        else
        {
            element **indirect = &head;
            for (size_t i = 0; i < pos; i++)
            {
                indirect = &((*indirect)->next);
            }
            element *one      = new element(e);
            one->last         = (*indirect)->last;
            one->next         = *indirect;
            (*indirect)->last = one;
            *indirect         = one;
            ++size;
        }
    }

    void remove() override
    {
        if (size <= 1)
        {
            if (size == 0)
            {
                return;
            }
            delete head;
            head = nullptr;
            tail = nullptr;
            size = 0;
        }
        else
        {
            tail->last->next = head;
            head->last       = tail->last;
            delete tail;
            tail = head->last;
            --size;
        }
    }

    void remove(const size_t &pos) override
    {
        if (size <= 1 || pos >= size - 1)
        {
            remove();
        }
        else
        {
            element **indirect = &head;
            element  *index    = nullptr;
            for (size_t i = 0; i < pos; i++)
            {
                indirect = &((*indirect)->next);
            }
            index             = (*indirect);
            index->next->last = index->last;
            *indirect         = (*indirect)->next;
            delete index;
            --size;
        }
    }

    size_t get_size() const override
    {
        return size;
    }

    T &get(size_t pos) const override
    {
        pos = pos >= size ? size : pos;
        return (*this)[pos];
    }

    T &operator[](size_t pos) const override
    {
        element *temp = head;
        for (size_t i = 0; i < pos; i++)
        {
            temp = temp->next;
        }
        return temp->value;
    }

    T &get_front() override
    {
        return head->value;
    }

    T &get_back() override
    {
        return tail->value;
    }

    bool is_empty() override
    {
        return size == 0;
    }

    mycpp::ListLink<T>::iterator begin()
    {
        return iterator(head);
    }

    mycpp::ListLink<T>::iterator end()
    {
        return iterator(nullptr);
    }

    void push(const T &ele_) override
    {
        add(ele_);
    }

    T pop() override
    {
        T temp = get_back();
        this->remove();
        return temp;
    }

    T &peek() override
    {
        return this->get_back();
    }
};

template <class T>
class list_array : public list<T>, public stack<T>
{
  private:
    T           *data;
    size_t       len{};
    size_t       cap{};
    const size_t increment = 64;

    void copy(T *s, T *d, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            *(d + i) = *(s + i);
        }
    }

    void move(T *s, int d, size_t size)
    {
        for (size_t i = size - 1; i >= 0; i--)
        {
            *(s + i + d) = *(s + i);
        }
    }

  public:
    class iterator
    {

        T     *zero;
        size_t size;
        size_t loc;

        iterator(T *zero_, size_t size_, size_t loc_)
            : zero(zero_), size(size_), loc(loc_)
        {
        }

      public:
        friend mycpp::list_array<T>;

        iterator() : zero(nullptr), size(0U), loc(0U)
        {
        }

        T *operator->()
        {
            return zero + loc;
        }

        T &operator*()
        {
            return *(zero + loc);
        }

        mycpp::list_array<T>::iterator &operator++()
        {
            if (this->loc >= this->size - 1)
            {
                this->zero = nullptr;
                this->loc  = 0U;
            }
            else
            {
                this->loc++;
            }
            return *this;
        }

        mycpp::list_array<T>::iterator &operator--()
        {
            if (this->loc == 0U)
            {
                this->zero = nullptr;
            }
            else
            {
                this->loc--;
            }
            return *this;
        }

        mycpp::list_array<T>::iterator &operator++(int)
        {
            return ++(*this);
        }

        mycpp::list_array<T>::iterator &operator--(int)
        {
            return --(*this);
        }

        bool operator==(const mycpp::list_array<T>::iterator &it)
        {
            return this->zero == it.zero && this->loc == it.loc;
        }

        bool operator!=(const mycpp::list_array<T>::iterator &it)
        {
            return this->zero != it.zero || this->loc != it.loc;
        }
    };

    explicit list_array(size_t cap = 64)
        : data(nullptr), len(0), cap(cap), increment(64)
    {
        data = new T[this->cap + increment];
    }

    list_array(const list_array &la)
    {
    }

    list_array(list_array &&la) noexcept
    {
    }

    list_array &operator=(const list_array &la) noexcept
    {
    }

    list_array &operator=(list_array &&la) noexcept
    {
    }

    ~list_array() = default;

    void add(const T &e) override
    {
        if (len < cap)
        {
            *(data + len) = e;
        }
        else
        {
            cap += increment;
            T *temp = new T[cap];
            copy(data, temp, len);
            delete[] data;
            data = temp;
        }
        ++len;
    }

    void add(const size_t &pos, const T &e) override
    {
        // pos = pos > (len - 1) ? (len - 1) : pos;
        if (len < cap)
        {
            move(data + pos, 1, len - 1 - pos);
            *(data + pos) = e;
        }
        else
        {
            cap += increment;
            T *temp = new T[cap];
            this->copy(data, temp, pos);
            this->copy(data + pos, temp + pos + 1, len - pos);
            *(temp + pos) = e;
            delete[] data;
            data = temp;
        }
        ++len;
    }

    void remove() override
    {
        --len;
    }

    void remove(const size_t &pos) override
    {
        size_t pos_ = pos > (len - 1) ? (len - 1) : pos;
        this->move(data + pos_ + 1, -1, len - 1 - pos_);
        --len;
    }

    size_t get_size() const override
    {
        return len;
    }

    T &get(size_t pos) const override
    {
        pos = pos > (len - 1) ? (len - 1) : pos;
        return *(data + pos);
    }

    T &operator[](size_t pos) const override
    {
        return *(data + pos);
    }

    void clean() override
    {
        len = 0;
    }

    T &get_front() override
    {
        return data[0];
    }

    T &get_back() override
    {
        return data[len];
    }

    T *get_data()
    {
        return data;
    }

    bool is_empty() override
    {
        return len == 0;
    }

    void push(const T &ele_) override
    {
        this->add(ele_);
    }

    T pop() override
    {
        T temp = get_back();
        this->remove();
        return temp;
    }

    T &peek() override
    {
        return this->get_back();
    }

    mycpp::list_array<T>::iterator begin()
    {
        return mycpp::list_array<T>::iterator(this->data, this->len, 0U);
    }

    mycpp::list_array<T>::iterator end()
    {
        return mycpp::list_array<T>::iterator();
    }
};

template <class T>
class slice
{
};

} // namespace mycpp