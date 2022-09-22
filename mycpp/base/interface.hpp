#pragma once

#if defined(linux) or defined(__linux) or defined(__linux__) or                \
    defined(unix) or defined(__unix) or defined(__unix__)
#define USE_LINUX
#elif defined(WIN32) or defined(_WIN64) or defined(_WIN32)
#define USE_WIN32
#endif

// 头文件
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <forward_list>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

// 平台头文件
#ifdef USE_LINUX

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#endif

#ifdef USE_WIN32

#include <io.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#endif // USE_WIN32

namespace mycpp
{

using u8  = unsigned char;
using s8  = char;
using u16 = unsigned short;
using s16 = short;
using u32 = unsigned int;
using s32 = int;
using u64 = unsigned long long;
using s64 = long long;
using f32 = float;
using f64 = double;

} // namespace mycpp
