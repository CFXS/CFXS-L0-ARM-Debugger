#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <QString>

namespace L0 {

    class Log {
    public:
        static void Initialize();

        static inline std::shared_ptr<spdlog::logger>& GetCoreLogger() {
            return s_CoreLogger;
        }

        static inline std::shared_ptr<spdlog::logger>& GetUILogger() {
            return s_UILogger;
        }

        static inline std::shared_ptr<spdlog::logger>& GetProjectLogger() {
            return s_ProjectLogger;
        }

        static inline std::shared_ptr<spdlog::logger>& GetProbeLogger() {
            return s_ProbeLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_UILogger;
        static std::shared_ptr<spdlog::logger> s_ProjectLogger;
        static std::shared_ptr<spdlog::logger> s_ProbeLogger;
    };

} // namespace L0

template<>
struct fmt::formatter<QString> {
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const QString& input, FormatContext& ctx) -> decltype(ctx.out()) {
        return format_to(ctx.out(), "{}", input.toStdString());
    }
};

// Core log macros
#define LOG_CORE_TRACE(...)    ::L0::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LOG_CORE_INFO(...)     ::L0::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_WARN(...)     ::L0::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_ERROR(...)    ::L0::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_CRITICAL(...) ::L0::Log::GetCoreLogger()->critical(__VA_ARGS__)

// UI log macros
#define LOG_UI_TRACE(...)    ::L0::Log::GetUILogger()->trace(__VA_ARGS__)
#define LOG_UI_INFO(...)     ::L0::Log::GetUILogger()->info(__VA_ARGS__)
#define LOG_UI_WARN(...)     ::L0::Log::GetUILogger()->warn(__VA_ARGS__)
#define LOG_UI_ERROR(...)    ::L0::Log::GetUILogger()->error(__VA_ARGS__)
#define LOG_UI_CRITICAL(...) ::L0::Log::GetUILogger()->critical(__VA_ARGS__)

// Project log macros
#define LOG_PROJECT_TRACE(...)    ::L0::Log::GetProjectLogger()->trace(__VA_ARGS__)
#define LOG_PROJECT_INFO(...)     ::L0::Log::GetProjectLogger()->info(__VA_ARGS__)
#define LOG_PROJECT_WARN(...)     ::L0::Log::GetProjectLogger()->warn(__VA_ARGS__)
#define LOG_PROJECT_ERROR(...)    ::L0::Log::GetProjectLogger()->error(__VA_ARGS__)
#define LOG_PROJECT_CRITICAL(...) ::L0::Log::GetProjectLogger()->critical(__VA_ARGS__)

// Probe log macros
#define LOG_PROBE_TRACE(...)    ::L0::Log::GetProbeLogger()->trace(__VA_ARGS__)
#define LOG_PROBE_INFO(...)     ::L0::Log::GetProbeLogger()->info(__VA_ARGS__)
#define LOG_PROBE_WARN(...)     ::L0::Log::GetProbeLogger()->warn(__VA_ARGS__)
#define LOG_PROBE_ERROR(...)    ::L0::Log::GetProbeLogger()->error(__VA_ARGS__)
#define LOG_PROBE_CRITICAL(...) ::L0::Log::GetProbeLogger()->critical(__VA_ARGS__)