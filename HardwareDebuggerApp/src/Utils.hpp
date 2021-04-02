#pragma once

#define HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(enumName, dataType)                            \
    inline enumName operator|(enumName a, enumName b) {                                    \
        return static_cast<enumName>(static_cast<dataType>(a) | static_cast<dataType>(b)); \
    }                                                                                      \
    inline bool operator&(enumName a, enumName b) {                                        \
        return static_cast<dataType>(a) & static_cast<dataType>(b);                        \
    }