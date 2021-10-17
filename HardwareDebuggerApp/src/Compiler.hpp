#pragma once

#if defined(CFXS_PLATFORM_WINDOWS)
#define HWD_DEBUGBREAK() __debugbreak()
#elif defined(CFXS_PLATFORM_LINUX)
#include <signal.h>
#define HWD_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform not defined"
#endif

#define __CLASS__ typeid(*this).name()