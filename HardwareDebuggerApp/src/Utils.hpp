#pragma once

#define DEF_REG_OPERATORS()                                                                                                               \
    inline void operator=(uint32_t val) {                                                                                                 \
        Probe::IProbe::GetCurrentProbe()->Target_WriteMemory_32(m_Address, val);                                                          \
    }                                                                                                                                     \
    inline void operator|=(uint32_t val) {                                                                                                \
        Probe::IProbe::GetCurrentProbe()->Target_WriteMemory_32(m_Address,                                                                \
                                                                Probe::IProbe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) | val); \
    }                                                                                                                                     \
    inline void operator&=(uint32_t val) {                                                                                                \
        Probe::IProbe::GetCurrentProbe()->Target_WriteMemory_32(m_Address,                                                                \
                                                                Probe::IProbe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) & val); \
    }                                                                                                                                     \
    inline uint32_t GetRaw() {                                                                                                            \
        return Probe::IProbe::GetCurrentProbe()->Target_ReadMemory_32(m_Address);                                                         \
    }

#define DEF_REG_CONSTRUCTOR(name)                 \
    name(uint32_t regAddr) : m_Address(regAddr) { \
    }                                             \
    name(const name& other) = delete;             \
    name(name&& other)      = delete;             \
    constexpr void SetAddress(uint32_t addr) {    \
        m_Address = addr;                         \
    }

#define DEF_REG_FIELD(name, highBitIndex, lowBitIndex)                                                                                     \
    static constexpr uint32_t MASK_##name  = (((0xFFFFFFFF >> lowBitIndex) << lowBitIndex) << (31 - highBitIndex)) >> (31 - highBitIndex); \
    static constexpr uint32_t SHIFT_##name = lowBitIndex;                                                                                  \
    inline uint32_t Get_##name() const {                                                                                                   \
        return (Probe::IProbe::GetCurrentProbe()->Target_ReadMemory_32(m_Address) & MASK_##name) >> SHIFT_##name;                          \
    }                                                                                                                                      \
    inline uint32_t GetRaw_##name(uint32_t val) const {                                                                                    \
        return (val & MASK_##name) >> SHIFT_##name;                                                                                        \
    }

#define HWD_OVERLOAD_ENUM_BITWISE_OPERATORS(enumName, dataType)                            \
    inline enumName operator|(enumName a, enumName b) {                                    \
        return static_cast<enumName>(static_cast<dataType>(a) | static_cast<dataType>(b)); \
    }                                                                                      \
    inline bool operator&(enumName a, enumName b) {                                        \
        return static_cast<dataType>(a) & static_cast<dataType>(b);                        \
    }