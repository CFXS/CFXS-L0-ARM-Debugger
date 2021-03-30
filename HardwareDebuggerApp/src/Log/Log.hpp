#pragma once

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace HWD {

    class Log {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetCoreLogger() {
            return s_CoreLogger;
        }

    private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
    };

} // namespace HWD