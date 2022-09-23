#pragma once

#include <cstddef>

#if defined(CONFIG_LIBCXXMINI)
#define USE_MIWEAR_CXX_LIB
#endif

#if defined(USE_MIWEAR_CXX_LIB)

FAR void *operator new(std::size_t nbytes, FAR void *ptr);

FAR void *operator new[](std::size_t nbytes, FAR void *ptr);

template <typename T>
T &&Move(const T &arg)
{
    return const_cast<T &&>(arg);
}

#endif
