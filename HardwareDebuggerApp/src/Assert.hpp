#pragma once

#include <filesystem>

#define HWD_ASSERT(x, y)                                                                                                       \
    {                                                                                                                          \
        if (!(x)) {                                                                                                            \
            HWD_CORE_ERROR("{0}:{1}  ASSERT FAILED: " #y "\n", std::filesystem::path(__FILE__).filename().string(), __LINE__); \
            HWD_DEBUGBREAK();                                                                                                  \
        }                                                                                                                      \
    }
