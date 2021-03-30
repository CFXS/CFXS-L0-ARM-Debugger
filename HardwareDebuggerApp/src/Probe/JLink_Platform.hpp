#pragma once

#if defined(HWD_PLATFORM_WINDOWS)
#include <Windows.h>
inline auto _JLink_LoadLibrary() {
    return LoadLibrary("JLink_x64.dll");
}
template<typename FunctionSignature>
inline auto _JLink_GetFunction(HINSTANCE lib, LPCSTR funcName) {
    return static_cast<FunctionSignature>(GetProcAddress(lib, funcName));
}
#else
#error "Unsupported platform"
#endif