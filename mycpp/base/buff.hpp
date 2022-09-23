#pragma once

#include "mycpp/base/interface.hpp"

namespace mycpp
{

template <class T>
class basic_buff : public std::basic_streambuf<T>
{

    using int_type = typename std::basic_streambuf<T>::int_type;

    static const size_t WRITER_BUF_SIZE = 512;
    static const size_t READER_BUF_SIZE = 512;

  private:
    FILE *reader;
    FILE *writer;

#ifdef USE_LINUX
    int r;
    int w;
#endif

#ifdef USE_WIN32
    ::HANDLE r;
    ::HANDLE w;
#endif

    T writer_buf[WRITER_BUF_SIZE];
    T reader_buf[READER_BUF_SIZE];

    bool is_init;

  protected:
    int_type flush_buffer()
    {
        size_t len = this->pptr() - this->pbase();
#ifdef USE_LINUX
        size_t num = write(w, writer_buf, len * sizeof(T));
        if (num <= 0)
        {
            return EOF;
        }
#endif

#ifdef USE_WIN32
        DWORD num = 0;
        if (::WriteFile(w, writer_buf, len * sizeof(T), &num, NULL))
        {
            return EOF;
        }
#endif
        this->pbump(-num);
        return num;
    }

  public:
    basic_buff() : reader(nullptr), writer(nullptr), w(0), r(0), is_init(false)
    {

        this->setg(reader_buf, reader_buf + READER_BUF_SIZE,
                   reader_buf + READER_BUF_SIZE);

        this->setp(writer_buf, writer_buf + WRITER_BUF_SIZE - 1);
    }

    basic_buff(const basic_buff &) = delete;

    basic_buff(basic_buff &&) = delete;

    basic_buff &operator=(const basic_buff &) = delete;

    basic_buff &operator=(basic_buff &&) = delete;

    ~basic_buff()
    {
        if (is_init)
            sync();
    }

    virtual int_type overflow(int_type c) override
    {
        if (c != EOF)
        {
            *(this->pptr()) = c;
            this->pbump(1);
        }
        if (flush_buffer() == EOF)
        {
            return EOF;
        }
        return c;
    }

    virtual int sync() override
    {
        if (flush_buffer() == EOF)
        {
            return -1;
        }
        return 0;
    }

    virtual int_type underflow() override
    {

#ifdef USE_LINUX
        size_t num = read(r, (void *)reader_buf, READER_BUF_SIZE * sizeof(T));
        if (num <= 0)
        {
            return EOF;
        }
#endif
#ifdef USE_WIN32
        ::DWORD num = 0;
        if (::ReadFile(r, reader_buf, READER_BUF_SIZE, &num, NULL))
        {
            return EOF;
        }
#endif
        this->setg(reader_buf, reader_buf, reader_buf + num);
        return *(this->gptr());
    }

    virtual void dis_init()
    {
        is_init = false;
    }

  public:
#ifdef USE_LINUX

    basic_buff(int out_, int in_) : basic_buff()
    {
        init(out_, in_);
    }

    void init(int in_, int out_)
    {
        reader  = fdopen(in_, "r+");
        writer  = fdopen(out_, "w+");
        r       = in_;
        w       = out_;
        is_init = true;
    }

#endif

#ifdef USE_WIN32
    basic_buff(::HANDLE in_, ::HANDLE out_) : basic_buff()
    {
        init(in_, out_);
    }

    void init(::HANDLE in_, ::HANDLE out_)
    {
        r       = in_;
        w       = out_;
        is_init = true;
    }
#endif
};

using pipebuff   = basic_buff<char>;
using socketbuff = basic_buff<char>;
using serialbuff = basic_buff<char>;
} // namespace mycpp
