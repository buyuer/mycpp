#pragma once

#if defined(linux) or defined(__linux) or defined(__linux__) or defined(unix) or defined(__unix) or defined(__unix__)
#define USE_LINUX
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
#include "termios.h"

#endif