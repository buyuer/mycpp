#pragma once

#include <cstddef>
#include <cstring>

#include "Base.hpp"

#define STRING_DEFAULT_SIZE 2

template <typename CharT, CharT EOF_ = 0> size_t Strlen(const CharT* str)
{
    auto len = 0;
    while (*str++ != EOF_) {
        ++len;
    }
    return len;
}

template <typename CharT>
bool CompareStr(const CharT* str1, const CharT* str2, size_t str_size)
{
    for (size_t index = 0; index < str_size; ++index) {
        if (*(str1 + index) - *(str2 + index) != 0) {
            return false;
        }
    }
    return true;
}

template <typename CharT>
size_t FindSubStr(const CharT* str, size_t str_size, size_t pos,
                  const CharT* target, size_t target_size)
{
    str += pos;
    str_size -= pos;
    for (ssize_t index = 0;
         index < (ssize_t)str_size - (ssize_t)target_size + 1; ++index) {
        if (*(str + index) == *target) {
            auto compare = str + index;
            if (CompareStr(compare, target, target_size)) {
                return index + pos;
            }
        }
    }
    return -1;
}

template <typename CharT> class BasicString {
public:
    using size_type = size_t;

    using iterator = CharT*;

    using const_iterator = const CharT*;

    static const size_type npos = -1;

    static const CharT EOF_ = 0;

    BasicString()
    {
        cap_  = STRING_DEFAULT_SIZE;
        data_ = (CharT*)::malloc(SIZE_BYTE(cap_));
        set_eof();
    }

    BasicString(const CharT* str) { this->assign(str); }

    BasicString(const iterator first, const iterator last)
    {
        init_(first, last - first);
    }

    ~BasicString() { ::free(data_); }

    BasicString(const BasicString& other) { copy_(other); }

    BasicString(BasicString&& other)
    {
        move_(static_cast<BasicString&&>(other));
    }

    BasicString& operator=(const BasicString& other) { return copy_(other); }

    BasicString& operator=(BasicString&& other) noexcept
    {
        return move_(static_cast<BasicString&&>(other));
    }

    BasicString& assign(const CharT* str)
    {
        init_(str, Strlen(str));
        return *this;
    }

    /* 元素访问 */
    CharT& operator[](size_type pos) { return *(data_ + pos); }

    CharT& front() { return *data_; }

    const CharT& front() const { return *data_; }

    CharT& back() { return *(data_ + size_ - 1); }

    CharT& back() const { return *(data_ + size_ - 1); }

    CharT* data() noexcept { return data_; }

    CharT* data() const noexcept { return data_; }

    const CharT* c_str() const noexcept { return data_; }

    /* 容量 */
    bool empty() const { return size_ == 0; }

    size_type size() const { return size_; }

    size_type length() const { return size(); }

    /* 操作 */
    void clear()
    {
        ::memset(data_, EOF_, size_);
        size_ = 0;
    }

    void push_back(const CharT ch) { append_(&ch, 1); }

    void pop_back()
    {
        --size_;
        set_eof();
    }

    BasicString& operator+=(const BasicString& other)
    {
        if (other.size_ != 0) {
            append_(other.data_, other.size_);
        }
        return *this;
    }

    BasicString& operator+=(const CharT* str)
    {
        auto str_len = Strlen(str);
        if (str_len != 0) {
            append_(str, str_len);
        }
        return *this;
    }

    BasicString substr(size_type pos = 0, size_type count = npos) const
    {
        if (count > size_ - pos) {
            count = size_ - pos;
        }
        return BasicString(data_ + pos, data_ + pos + count);
    }

    /* 查找 */
    size_type find(const CharT c, size_type pos = 0) const
    {
        return FindSubStr<CharT>(data_, size_, pos, &c, 1);
    }

    size_type find(const CharT* s, size_type pos, size_type count) const
    {
        return FindSubStr<CharT>(data_, size_, pos, s, count);
    }

    size_type find(const CharT* s, size_type pos = 0) const
    {
        auto count = Strlen(s);
        return find(s, pos, count);
    }

    size_type find(const BasicString& str, size_type pos = 0) const
    {
        return FindSubStr<CharT>(data_, size_, pos, str.data_, str.size_);
    }

private:
    void init_(const CharT* str, size_type size)
    {
        size_ = size;
        cap_  = size_ + STRING_DEFAULT_SIZE;
        data_ = (CharT*)::malloc(SIZE_BYTE(cap_));
        ::memmove(data_, str, size_);
        set_eof();
    }

    BasicString& copy_(const BasicString& other)
    {
        size_ = other.size_;
        cap_  = other.cap_;
        data_ = (CharT*)::malloc(cap_);
        ::memcpy(data_, other.data_, size_);
        set_eof();
        return *this;
    }

    BasicString& move_(BasicString&& other)
    {
        size_ = other.size_;
        cap_  = other.cap_;
        data_ = other.data_;

        other.size_ = 0;
        other.cap_  = 0;
        other.data_ = nullptr;

        set_eof();
        return *this;
    }

    void append_(const CharT* str, size_type len)
    {
        auto move_pos = size_;
        size_         = size_ + len;
        if (size_ >= cap_) {
            cap_  = size_ + STRING_DEFAULT_SIZE;
            data_ = (CharT*)::realloc(data_, cap_);
        }
        ::memcpy(data_ + move_pos, str, len);
        set_eof();
    }

    inline static size_type SIZE_BYTE(size_type size)
    {
        return size * sizeof(CharT);
    }

    inline void set_eof() { *(data_ + size_) = EOF_; }

    CharT*    data_ { nullptr };
    size_type size_ { 0 };
    size_type cap_ { 0 };
};

template <typename T>
BasicString<T> operator+(const BasicString<T>& str1, const BasicString<T>& str2)
{
    BasicString<T> ret { str1 };
    ret += str2;
    return Move(ret);
}

template <typename T>
BasicString<T> operator+(const BasicString<T>& str1, const T* str2)
{
    BasicString<T> ret { str1 };
    ret += str2;
    return Move(ret);
}

template <typename T>
BasicString<T> operator+(const T* str1, const BasicString<T>& str2)
{
    BasicString<T> ret { str1 };
    ret += str2;
    return Move(ret);
}

template <typename T>
bool operator==(const BasicString<T>& str1, const BasicString<T>& str2)
{
    if (str1.size() != str2.size()) {
        return false;
    }
    return CompareStr(str1.c_str(), str2.c_str(), str1.size());
}

template <typename T> bool operator==(const BasicString<T>& str1, const T* str2)
{
    if (str1.size() != Strlen(str2)) {
        return false;
    }
    return CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T> bool operator==(const T* str1, const BasicString<T>& str2)
{
    return str2 == str1;
}

template <typename T>
bool operator!=(const BasicString<T>& str1, const BasicString<T> str2)
{
    if (str1.size() != str2.size()) {
        return true;
    }
    return !CompareStr(str1.c_str(), str2.c_str(), str1.size());
}

template <typename T> bool operator!=(const BasicString<T>& str1, const T* str2)
{
    if (str1.size() != Strlen(str2)) {
        return true;
    }
    return !CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T> bool operator!=(const T* str1, const BasicString<T>& str2)
{
    return str2 != str1;
}

using String = BasicString<char>;

template <typename CharT> class BasicStringView {
public:
    using size_type = size_t;

    static const size_type npos = -1;

    static const CharT EOF_ = 0;

    BasicStringView() { }

    BasicStringView(const CharT* str) { init_(str, Strlen(str)); }

    BasicStringView(const BasicString<CharT>& str)
    {
        init_(str.data(), str.size());
    }

    /* 元素访问 */
    const CharT& operator[](size_type pos) { return *(data_ + pos); }

    const CharT& front() { return *data_; }

    const CharT& front() const { return *data_; }

    const CharT& back() { return *(data_ + size_ - 1); }

    const CharT& back() const { return *(data_ + size_ - 1); }

    const CharT* data() noexcept { return data_; }

    const CharT* data() const noexcept { return data_; }

    const CharT* c_str() const noexcept { return data_; }

    /* 容量 */
    bool empty() const { return size_ == 0; }

    size_type size() const { return size_; }

    size_type length() const { return size(); }

    /* 查找 */
    size_type find(const CharT c, size_type pos = 0) const
    {
        return FindSubStr<CharT>(data_, size_, pos, &c, 1);
    }

    size_type find(const BasicStringView& str, size_type pos = 0) const
    {
        return FindSubStr<CharT>(data_, size_, pos, str.data_, str.size_);
    }

private:
    void init_(const CharT* data, size_type size)
    {
        data_ = data;
        size_ = size;
    }

    const CharT* data_ { nullptr };
    size_type    size_ { 0 };
};

template <typename T>
bool operator==(const BasicStringView<T>& str1, const BasicStringView<T>& str2)
{
    if (str1.size() != str2.size()) {
        return false;
    }
    return CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T>
bool operator==(const BasicStringView<T>& str1, const T* str2)
{
    if (str1.size() != Strlen(str2)) {
        return false;
    }
    return CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T>
bool operator==(const T* str1, const BasicStringView<T>& str2)
{
    return str2 == str1;
}

template <typename T>
bool operator!=(const BasicStringView<T>& str1, const BasicStringView<T>& str2)
{
    if (str1.size() != str2.size()) {
        return true;
    }
    return !CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T>
bool operator!=(const BasicStringView<T>& str1, const T* str2)
{
    if (str1.size() != Strlen(str2)) {
        return true;
    }
    return !CompareStr(str1.c_str(), str2, str1.size());
}

template <typename T>
bool operator!=(const T* str1, const BasicStringView<T>& str2)
{
    return str2 != str1;
}

using StringView = BasicStringView<char>;