#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace HWD {

    class Log {
    public:
        static void Initialize();

        static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() {
            return s_CoreLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
    };

} // namespace HWD

// Core log macros
#define HWD_CORE_TRACE(...)    ::HWD::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HWD_CORE_INFO(...)     ::HWD::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HWD_CORE_WARN(...)     ::HWD::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HWD_CORE_ERROR(...)    ::HWD::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HWD_CORE_CRITICAL(...) ::HWD::Log::GetCoreLogger()->critical(__VA_ARGS__)