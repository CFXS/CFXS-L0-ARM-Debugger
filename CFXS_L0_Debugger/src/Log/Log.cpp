// ---------------------------------------------------------------------
// CFXS L0 ARM Debugger <https://github.com/CFXS/CFXS-L0-ARM-Debugger>
// Copyright (C) 2022 | CFXS
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>
// ---------------------------------------------------------------------
// [CFXS] //
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