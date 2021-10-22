#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

#include <QString>

namespace HWD {

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

} // namespace HWD

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
#define HWDLOG_CORE_TRACE(...)    ::HWD::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define HWDLOG_CORE_INFO(...)     ::HWD::Log::GetCoreLogger()->info(__VA_ARGS__)
#define HWDLOG_CORE_WARN(...)     ::HWD::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define HWDLOG_CORE_ERROR(...)    ::HWD::Log::GetCoreLogger()->error(__VA_ARGS__)
#define HWDLOG_CORE_CRITICAL(...) ::HWD::Log::GetCoreLogger()->critical(__VA_ARGS__)

// UI log macros
#define HWDLOG_UI_TRACE(...)    ::HWD::Log::GetUILogger()->trace(__VA_ARGS__)
#define HWDLOG_UI_INFO(...)     ::HWD::Log::GetUILogger()->info(__VA_ARGS__)
#define HWDLOG_UI_WARN(...)     ::HWD::Log::GetUILogger()->warn(__VA_ARGS__)
#define HWDLOG_UI_ERROR(...)    ::HWD::Log::GetUILogger()->error(__VA_ARGS__)
#define HWDLOG_UI_CRITICAL(...) ::HWD::Log::GetUILogger()->critical(__VA_ARGS__)

// Project log macros
#define HWDLOG_PROJECT_TRACE(...)    ::HWD::Log::GetProjectLogger()->trace(__VA_ARGS__)
#define HWDLOG_PROJECT_INFO(...)     ::HWD::Log::GetProjectLogger()->info(__VA_ARGS__)
#define HWDLOG_PROJECT_WARN(...)     ::HWD::Log::GetProjectLogger()->warn(__VA_ARGS__)
#define HWDLOG_PROJECT_ERROR(...)    ::HWD::Log::GetProjectLogger()->error(__VA_ARGS__)
#define HWDLOG_PROJECT_CRITICAL(...) ::HWD::Log::GetProjectLogger()->critical(__VA_ARGS__)

// Probe log macros
#define HWDLOG_PROBE_TRACE(...)    ::HWD::Log::GetProbeLogger()->trace(__VA_ARGS__)
#define HWDLOG_PROBE_INFO(...)     ::HWD::Log::GetProbeLogger()->info(__VA_ARGS__)
#define HWDLOG_PROBE_WARN(...)     ::HWD::Log::GetProbeLogger()->warn(__VA_ARGS__)
#define HWDLOG_PROBE_ERROR(...)    ::HWD::Log::GetProbeLogger()->error(__VA_ARGS__)
#define HWDLOG_PROBE_CRITICAL(...) ::HWD::Log::GetProbeLogger()->critical(__VA_ARGS__)