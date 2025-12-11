#pragma once
#include <spdlog/spdlog.h>
#include <memory.h>

namespace ke
{
    namespace util
    {
        class Logger
        {
        public:
            Logger() = default;
            Logger(const char* name, spdlog::level::level_enum level);

            static void initLoggers();

            void trace(const char* msg) const;
            void debug(const char* msg) const;
            void info(const char* msg) const;
            void warn(const char* msg) const;
            void error(const char* msg) const;
            void critical(const char* msg) const;
        private:
            std::shared_ptr<spdlog::logger> mLogger;
        };
    }
}