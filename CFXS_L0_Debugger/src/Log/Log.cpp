#include "Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "AppLogBufferSink.hpp"

namespace L0 {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_UILogger;
    std::shared_ptr<spdlog::logger> Log::s_ProjectLogger;
    std::shared_ptr<spdlog::logger> Log::s_ProbeLogger;

    void Log::Initialize() {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<L0_AppLogBufferSink>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("L0.log", true));

        logSinks[0]->set_pattern("%^[%T.%e] %n %v%$");
        logSinks[1]->set_pattern("%^[%T.%e] %n %v%$");
        logSinks[2]->set_pattern("%^[%T.%e] %n %v%$");

        s_CoreLogger = std::make_shared<spdlog::logger>("L0:     ", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);

        s_UILogger = std::make_shared<spdlog::logger>("L0-UI:  ", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_UILogger);
        s_UILogger->set_level(spdlog::level::trace);
        s_UILogger->flush_on(spdlog::level::trace);

        s_ProjectLogger = std::make_shared<spdlog::logger>("L0-Proj:", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ProjectLogger);
        s_ProjectLogger->set_level(spdlog::level::trace);
        s_ProjectLogger->flush_on(spdlog::level::trace);

        s_ProbeLogger = std::make_shared<spdlog::logger>("L0-Probe:", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ProbeLogger);
        s_ProbeLogger->set_level(spdlog::level::trace);
        s_ProbeLogger->flush_on(spdlog::level::trace);
    }

} // namespace L0