#include "Log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace HWD {

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ProbeLogger;

    void Log::Initialize() {
        std::vector<spdlog::sink_ptr> logSinks;
        logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("HWD.log", true));

        logSinks[0]->set_pattern("%^[%T.%e] %n: %v%$");
        logSinks[1]->set_pattern("[%T.%e] [%l] %n: %v");

        s_CoreLogger = std::make_shared<spdlog::logger>("HWD", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_CoreLogger);
        s_CoreLogger->set_level(spdlog::level::trace);
        s_CoreLogger->flush_on(spdlog::level::trace);

        s_ProbeLogger = std::make_shared<spdlog::logger>("HWD-Probe", begin(logSinks), end(logSinks));
        spdlog::register_logger(s_ProbeLogger);
        s_ProbeLogger->set_level(spdlog::level::trace);
        s_ProbeLogger->flush_on(spdlog::level::trace);
    }

} // namespace HWD