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

        static inline std::shared_ptr<spdlog::logger>& GetProbeLogger() {
            return s_CoreLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ProbeLogger;
    };

} // namespace HWD

// Core log macros
#define HWDLOG_CORE_TRACE(...)    ::HWD::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HWDLOG_CORE_INFO(...)     ::HWD::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HWDLOG_CORE_WARN(...)     ::HWD::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HWDLOG_CORE_ERROR(...)    ::HWD::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HWDLOG_CORE_CRITICAL(...) ::HWD::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Core log macros
#define HWDLOG_PROBE_TRACE(...)    ::HWD::Log::GetProbeLogger()->trace(__VA_ARGS__)
#define HWDLOG_PROBE_INFO(...)     ::HWD::Log::GetProbeLogger()->info(__VA_ARGS__)
#define HWDLOG_PROBE_WARN(...)     ::HWD::Log::GetProbeLogger()->warn(__VA_ARGS__)
#define HWDLOG_PROBE_ERROR(...)    ::HWD::Log::GetProbeLogger()->error(__VA_ARGS__)
#define HWDLOG_PROBE_CRITICAL(...) ::HWD::Log::GetProbeLogger()->critical(__VA_ARGS__)