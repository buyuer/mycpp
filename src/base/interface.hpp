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

#include <iostream>
#include <string>

//平台头文件
#ifdef USE_LINUX

#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <termios.h>
#include <netinet/in.h>

#endif




#ifdef USE_WIN32

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>

#endif // USE_WIN32
