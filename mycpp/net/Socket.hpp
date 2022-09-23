#pragma once

#include "mycpp/base/base.hpp"

namespace mycpp
{

class Socket
{

  public:
    enum TYPE : int
    {
        IPV4   = AF_INET,
        IPV6   = AF_INET6,
        TCP    = IPPROTO_TCP,
        UDP    = IPPROTO_UDP,
        STREAM = SOCK_STREAM,
        DGRAM  = SOCK_DGRAM,
    };

  private:
    struct element
    {
        int          rec_num;
        int          socket_fd;
        sockaddr_in6 addr;

        socketbuff    sockbuf;
        std::iostream io;
        std::string   ip;
        int           port;
        TYPE          af;

        element()
            : rec_num(0), socket_fd(0), io(&sockbuf), port(0),
              af(mycpp::Socket::IPV4)
        {
            memset(&addr, 0, sizeof(sockaddr_in6));
        }

        ~element()
        {
            ::close(socket_fd);
            sockbuf.dis_init();
        }
    };

    element *sock;

  private:
    void init(const int sock_fd_)
    {
        sock->socket_fd = sock_fd_;
        sock->sockbuf.init(sock->socket_fd, sock->socket_fd);
    }

    explicit Socket(const int sock_fd_) : sock(new element)
    {
        sock->rec_num = 1;
        init(sock_fd_);
    }

    explicit Socket() : sock(new element)
    {
        sock->rec_num = 1;
    }

  public:
    Socket(const Socket &s)
    {
        this->sock = s.sock;
        this->sock->rec_num += 1;
    }

    Socket(Socket &&s)
    {
        this->sock = s.sock;
        this->sock->rec_num += 1;
    }

    Socket &operator=(const Socket &s)
    {
        this->sock->rec_num -= 1;
        this->sock = s.sock;
        this->sock->rec_num += 1;
        return *this;
    }

    Socket &operator=(Socket &&s)
    {
        this->sock->rec_num -= 1;
        this->sock = s.sock;
        this->sock->rec_num += 1;
        return *this;
    }

    ~Socket()
    {
        sock->rec_num -= 1;
        if (sock->rec_num <= 0)
        {
            delete sock;
        }
    }

  public:
    Socket(TYPE inet, TYPE type = STREAM, TYPE protocol = TCP) : sock(nullptr)
    {
        sock            = new element();
        sock->socket_fd = ::socket(inet, type, protocol);
        if (sock->socket_fd == -1)
        {
            perror("socket failed");
            exit(0);
        }
        sock->sockbuf.init(sock->socket_fd, sock->socket_fd);
        sock->addr.sin6_family = inet;
        sock->af               = inet;
        sock->rec_num          = 1;
    }

    void bind(std::string ip, int port)
    {
        if (((sockaddr *)&sock->addr)->sa_family == IPV4)
        {
            sockaddr_in &addr    = *(sockaddr_in *)(&sock->addr);
            addr.sin_family      = IPV4;
            addr.sin_port        = ::htons(port);
            addr.sin_addr.s_addr = ::inet_addr(ip.data());

            if (::bind(sock->socket_fd, (sockaddr *)&addr,
                       sizeof(sockaddr_in)) == -1)
            {
                perror("bind failed");
                exit(0);
            }
        }
        else if (((sockaddr *)&sock->addr)->sa_family == IPV6)
        {
            sockaddr_in6 &addr = *(sockaddr_in6 *)(&sock->addr);
            addr.sin6_family   = IPV6;
            addr.sin6_port     = ::htons(port);
            if (inet_pton(IPV6, ip.data(), addr.sin6_addr.__in6_u.__u6_addr8) <
                0)
            {
                perror("inet_pton failed");
                exit(0);
            }

            if (::bind(sock->socket_fd, (sockaddr *)&addr,
                       sizeof(sockaddr_in6)) == -1)
            {
                perror("bind failed");
                exit(0);
            }
        }
    }

    void listen(int backlog = SOMAXCONN)
    {
        if (::listen(sock->socket_fd, backlog) == -1)
        {
            perror("listen failed");
            exit(0);
        }
    }

    mycpp::Socket accept()
    {

        mycpp::Socket temp;
        ::socklen_t   addr_len;
        if (this->sock->af == mycpp::Socket::IPV4)
        {
            addr_len = sizeof(::sockaddr_in);
        }
        else if (this->sock->af == mycpp::Socket::IPV6)
        {
            addr_len = sizeof(::sockaddr_in6);
        }

        int client_socket = ::accept(sock->socket_fd,
                                     (sockaddr *)&(temp.sock->addr), &addr_len);

        if (client_socket == -1)
        {
            perror("accept failed");
        }

        char ipbuff[64];
        inet_ntop(this->sock->af, &(temp.sock->addr), ipbuff, addr_len);
        std::string ip(ipbuff, std::strlen(ipbuff));

        temp.sock->ip   = std::move(ip);
        temp.sock->port = ::ntohl(this->sock->addr.sin6_port);
        temp.sock->af   = this->sock->af;

        temp.init(client_socket);

        return temp;
    }

    void connect(std::string ip, int port)
    {

        sockaddr_in addr;
        memset(&addr, 0, sizeof(sockaddr_in));

        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = inet_addr(ip.data());
        addr.sin_port        = htons(port);

        if (::connect(sock->socket_fd, (sockaddr *)&addr, sizeof(addr)) == -1)
        {
            perror("connect failed");
            exit(0);
        }
    }

    void close()
    {
        ::close(this->sock->socket_fd);
    }

    inline int get_socket()
    {
        return sock->socket_fd;
    }

    std::iostream &io()
    {
        return sock->io;
    }

    std::string &get_ip()
    {
        return this->sock->ip;
    }

    int get_port()
    {
        return this->sock->port;
    }
};
} // namespace mycpp

#ifdef USE_WIN32

#pragma comment(lib, "ws2_32.lib")

namespace mycpp
{

class WinSocket
{
  private:
    int       _family;
    int       _socktype;
    int       _protocol;
    int       _addrlen;
    SOCKADDR *_addr;
    SOCKET    _socket;

  protected:
  public:
    using TYPE = enum
    {
        INET4  = AF_INET,
        INET6  = AF_INET6,
        STREAM = SOCK_STREAM,
        DGRAM  = SOCK_DGRAM,
    };

    static void init()
    {
        WSADATA wsaData;
        WORD    sockVersion = MAKEWORD(2, 2);
        if (WSAStartup(sockVersion, &wsaData) != 0)
        {
            return;
        }
    }

    explicit WinSocket(int pf = STREAM)
        : _socket(INVALID_SOCKET), _family(NULL), _socktype(NULL),
          _protocol(NULL), _addrlen(NULL), _addr(NULL)
    {
        _socktype = pf;
    };

    WinSocket(const WinSocket &wsock) = delete;

    WinSocket(WinSocket &&wsock) noexcept : WinSocket()
    {
        memcpy(this, &wsock, sizeof(WinSocket));
        wsock._socket = INVALID_SOCKET;
        wsock._addr   = NULL;
    }

    WinSocket &operator=(const WinSocket &wsock) = delete;

    WinSocket &operator=(WinSocket &&wsock) noexcept
    {
        memcpy(this, &wsock, sizeof(WinSocket));
        wsock._socket = INVALID_SOCKET;
        wsock._addr   = NULL;
    }

    ~WinSocket()
    {
        if (_socket != INVALID_SOCKET)
        {
            this->close();
        }
        if (_addr != NULL)
        {
            delete[] _addr;
        }
    };

    bool bind(string ip, int port)
    {
        ADDRINFO *result = NULL, hints;
        ZeroMemory(&hints, sizeof(hints));
        // hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = _socktype;
        hints.ai_protocol = _protocol;
        if (getaddrinfo(ip.data(), to_string(port).data(), &hints, &result) !=
            0)
        {
            cout << "ip or port is error" << endl;
            return false;
        }
        _socket =
            socket(result->ai_family, result->ai_socktype, result->ai_protocol);
        if (_socket == INVALID_SOCKET)
        {
            cout << "socket failed" << endl;
            freeaddrinfo(result);
            return false;
        }
        if (_socktype == SOCK_STREAM)
        {
            if (::bind(_socket, result->ai_addr, result->ai_addrlen) ==
                SOCKET_ERROR)
            {
                cout << "bind error\n" << endl;
                return false;
            }
        }
        _family  = result->ai_family;
        _addrlen = result->ai_addrlen;
        _addr    = (SOCKADDR *)(new char[_addrlen]);
        _addr    = (SOCKADDR *)std::memcpy(_addr, result->ai_addr, _addrlen);

        freeaddrinfo(result);
        return true;
    }

    bool listen(int backlog = SOMAXCONN)
    {
        if (::listen(_socket, backlog) == SOCKET_ERROR)
        {
            cout << "listen error" << endl;
            return false;
        }
        return true;
    }

    bool connect()
    {
        if (::connect(_socket, _addr, _addrlen) == SOCKET_ERROR)
        {
            cout << "connect error" << endl;
            return false;
        }
    }

    bool connect(string ip, int port)
    {
        ADDRINFO hints, *result = NULL;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_protocol = _protocol;
        hints.ai_socktype = _socktype;
        if (getaddrinfo(ip.data(), to_string(port).data(), &hints, &result) !=
            0)
        {
            cout << "ip or port is error" << endl;
            freeaddrinfo(result);
            return false;
        }
        _socket = ::socket(result->ai_family, result->ai_socktype,
                           result->ai_protocol);
        if (_socket == INVALID_SOCKET)
        {
            cout << "socket failed" << endl;
            freeaddrinfo(result);
            return false;
        }
        if (::connect(_socket, result->ai_addr, result->ai_addrlen) ==
            SOCKET_ERROR)
        {
            cout << "connect error" << endl;
            freeaddrinfo(result);
            return false;
        };
        freeaddrinfo(result);
        return true;
    }

    WinSocket accept()
    {
        WinSocket temp(_socktype);
        memcpy(&temp, this, sizeof(WinSocket));
        temp._addr   = (SOCKADDR *)(new char[_addrlen]);
        temp._socket = ::accept(_socket, temp._addr, NULL);
        if (temp._socket == INVALID_SOCKET)
        {
            closesocket(_socket);
            cout << "accept error" << endl;
            temp._socket = INVALID_SOCKET;
            return temp;
        }
        return temp;
    }

    int send(const char *data, int n)
    {
        return ::send(_socket, data, n, NULL);
    }

    int recv(char *buf, int n)
    {
        return ::recv(_socket, buf, n, NULL);
    }

    int sendto(const char *buf, int n)
    {
        return ::sendto(_socket, buf, n, NULL, _addr, _addrlen);
    }

    int sendto(const char *buf, int n, string ip, int port)
    {
    }

    int recvfrom(char *buf, int n)
    {
        return ::recvfrom(_socket, buf, n, NULL, _addr, &_addrlen);
    }

    int recvfrom(char *buf, int n, string ip, int port)
    {
    }

    void close()
    {
        ::closesocket(_socket);
        _socket = INVALID_SOCKET;
    }

    string getip()
    {
    }

    int getport()
    {
    }
};

} // namespace mycpp

#endif