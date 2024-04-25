module;

#include <streambuf>

#if defined(linux) or defined(__linux) or defined(__linux__) or                \
    defined(unix) or defined(__unix) or defined(__unix__)
#define USE_LINUX
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>
#elif defined(WIN32) or defined(_WIN64) or defined(_WIN32)
#define USE_WIN32
#include <io.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

export module mycpp.base:buff;

export namespace mycpp
{

template <class T>
class BasicBuff : public std::basic_streambuf<T>
{
    using int_type = typename std::basic_streambuf<T>::int_type;

    static constexpr size_t WRITER_BUF_SIZE = 512;
    static constexpr size_t READER_BUF_SIZE = 512;

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
    BasicBuff() : reader(nullptr), writer(nullptr), w(0), r(0), is_init(false)
    {

        this->setg(reader_buf, reader_buf + READER_BUF_SIZE,
                   reader_buf + READER_BUF_SIZE);

        this->setp(writer_buf, writer_buf + WRITER_BUF_SIZE - 1);
    }

    BasicBuff(const BasicBuff &) = delete;

    BasicBuff(BasicBuff &&) = delete;

    BasicBuff &operator=(const BasicBuff &) = delete;

    BasicBuff &operator=(BasicBuff &&) = delete;

    ~BasicBuff() override
    {
        if (is_init)
            sync();
    }

    int_type overflow(int_type c) override
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

    int sync() override
    {
        if (flush_buffer() == EOF)
        {
            return -1;
        }
        return 0;
    }

    int_type underflow() override
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

    BasicBuff(int out_, int in_) : BasicBuff()
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
    BasicBuff(::HANDLE in_, ::HANDLE out_) : BasicBuff()
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

using pipebuff   = BasicBuff<char>;
using socketbuff = BasicBuff<char>;
using serialbuff = BasicBuff<char>;

} // namespace mycpp
