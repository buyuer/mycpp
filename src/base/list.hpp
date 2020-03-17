#pragma once

//为数据结构提供存储方式

template<class T>
class list {
    list() {}

    virtual ~list() = 0;
};


template<class T>
class list_link : public list<T> {
    struct element {
        T value;
        element *last;
        element *next;

        element(const T &d) :
                last(nullptr),
                next(nullptr),
                value(d) {}
    };

    size_t size;
    element *head;
    element *tail;

public:
    /*iterator end应该指向空，而不是最后一个*/
    class iterator {
        element *rec;
    public:

        iterator(element *p) : rec(p) {}

        iterator() : rec(nullptr) {}

        iterator &operator++() {
            rec = rec->next;
            return *this;
        }

        iterator &operator--() {
            rec = rec->last;
            return *this;
        }

        T &operator*() {
            return rec->value;
        }

        bool operator!=(const iterator &b) const {
            return b.rec != this->rec;
        }

        bool operator==(const iterator &b) const {
            return b.rec == this->rec;
        }
    };

    list_link() :
            size(0),
            head(nullptr),
            tail(nullptr) {}

    list_link(const list_link<T> &ll) :
            list_link() {

        size = ll.size;
        element **indirect = &head;
        element *index = ll.head;
        element *last = nullptr;
        for (size_t i = 0; i < size; i++) {
            *indirect = new element(index->value);
            (*indirect)->last = last;
            last = *indirect;
            indirect = &(*indirect)->next;
            index = index->next;
        }
        tail = last;
        (*indirect) = head;
        head->last = last;
    }

    list_link(list_link<T> &&ll) {
        size = ll.size;
        head = ll.head;
        tail = ll.tail;
        ll.size = 0;
        ll.head = nullptr;
        ll.tail = nullptr;
    }

    list_link &operator=(const list_link<T> &ll) noexcept {
        clean();
        new(this)list_link(ll);
        return *this;
    }

    list_link &operator=(list_link<T> &&ll) {
        clean();
        new(this)list_link(static_cast<list_link<T> &&>(ll));
        return *this;
    }

    ~list_link() {
        if (size == 0 || head == nullptr) {
            return;
        } else {
            clean();
        }
    };

    void clean() {
        element *index = head;
        for (size_t i = 0; i < size; i++) {
            element *temp = index;
            index = index->next;
            delete temp;
        }
        size = 0;
        head = nullptr;
        tail = nullptr;
    }

    void add(const T &e) {
        if (head == nullptr) {
            head = new element(e);
            head->next = head;
            head->last = head;
            tail = head;
        } else {
            element *temp = new element(e);
            temp->last = tail;
            temp->next = head;
            tail->next = temp;
            tail = temp;
            head->last = tail;
        }
        ++size;
    }

    void add(const size_t &pos, const T &e) {
        if (size == 0 || pos >= size - 1) {
            add(e);
        } else {
            element **indirect = &head;
            for (size_t i = 0; i < pos; i++) {
                indirect = &((*indirect)->next);
            }
            element *one = new element(e);
            one->last = (*indirect)->last;
            one->next = *indirect;
            (*indirect)->last = one;
            *indirect = one;
            ++size;
        }
    }

    void remove() {
        if (size <= 1) {
            if (size == 0) {
                return;
            }
            delete head;
            head = nullptr;
            tail = nullptr;
            size = 0;
        } else {
            tail->last->next = head;
            head->last = tail->last;
            delete tail;
            tail = head->last;
            --size;
        }
    }

    void remove(const size_t &pos) {
        if (size <= 1 || pos >= size - 1) {
            remove();
        } else {
            element **indirect = &head;
            element *index = nullptr;
            for (size_t i = 0; i < pos; i++) {
                indirect = &((*indirect)->next);
            }
            index = (*indirect);
            index->next->last = index->last;
            *indirect = (*indirect)->next;
            delete index;
            --size;
        }
    }

    size_t get_size() {
        return size;
    }

    T &get(size_t pos) {
        pos = pos >= size ? size : pos;
        return (*this)[pos];
    }

    T &operator[](size_t pos) {
        element *temp = head;
        for (size_t i = 0; i < pos; i++) {
            temp = temp->next;
        }
        return temp->value;
    }

    T &get_front() {
        return head->value;
    }

    T &get_back() {
        return tail->value;
    }

    iterator begin() {
        return iterator(head);
    }

    iterator end() {
        return iterator(tail);
    }
};


template<class T>
class list_array : public list<T> {
private:
    T *data;
    size_t len;
    size_t cap;
    static const size_t increment;

    void copy(T *s, T *d, size_t size) {
        for (size_t i = 0; i < size; i++) {
            *(d + i) = *(s + i);
        }
    }

    void move(T *s, int d, size_t size) {
        for (size_t i = size - 1; i >= 0; i--) {
            *(s + i + d) = *(s + i);
        }
    }

public:
    explicit list_array(size_t cap = 64) : data(nullptr), len(0), cap(cap), increment(64) {
        data = new T[this->cap + increment];
    }

    list_array(const list_array &la) {}

    list_array(list_array &&la) {}

    list_array &operator=(const list_array &la) {}

    list_array &operator=(list_array &&la) {}

    ~list_array() {}

    void add(const T &e) {
        if (len < cap) {
            *(data + len) = e;
        } else {
            cap += increment;
            T *temp = new T[cap];
            copy(data, temp, len);
            delete[]data;
            data = temp;
        }
        ++len;
    }

    void add(const size_t &pos, const T &e) {
        pos = pos > (len - 1) ? (len - 1) : pos;
        if (len < cap) {
            move(data + pos, 1, len - 1 - pos);
            *(data + pos) = e;
        } else {
            cap += increment;
            T *temp = new[cap];
            copy(data, temp, pos);
            copy(data + pos, temp + pos + 1, len - pos);
            *(temp + pos) = e;
            delete[] data;
            data = temp;
        }
        ++len;
    }

    void remove(const size_t &pos) {
        pos = pos > (len - 1) ? (len - 1) : pos;
        move(data + pos + 1, -1, len - 1 - pos);
        --len;
    }

    T &get(size_t pos) {
        pos = pos > (len - 1) ? (len - 1) : pos;
        return *(data + pos);
    }

    T &operator[](size_t pos) {
        return *(data + pos);
    }

    T *get_data() {
        return data;
    }
};


template<class T>
class slice {

};
