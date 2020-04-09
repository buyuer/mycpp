#pragma once

#if defined(linux) or defined(__linux) or defined(__linux__) or defined(unix) or defined(__unix) or defined(__unix__)
#define USE_LINUX
#elif defined(WIN32) or defined(_WIN64) or defined(_WIN32)
#define USE_WIN32
#endif

//头文件
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

//平台头文件
#ifdef USE_LINUX

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <termios.h>

#endif


#ifdef USE_WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>

#endif // USE_WIN32

namespace my {

    using u8 = unsigned char;
    using s8 = char;
    using u16 = unsigned short;
    using s16 = short;
    using u32 = unsigned int;
    using s32 = int;
    using u64 = unsigned long long;
    using s64 = long long;
    using f32 = float;
    using f64 = double;

}
