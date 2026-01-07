#include "Logger.hpp"

ke::util::Logger::Logger(const char *name)
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    mLogger = std::make_shared<spdlog::logger>(name, console_sink);
}

void ke::util::Logger::initLoggers()
{
    spdlog::set_pattern("%^[%H:%M:%S] %n: %v%$");
    spdlog::set_level(spdlog::level::debug);
}

void ke::util::Logger::trace(const char *msg) const
{
#ifdef DEBUG
    mLogger->trace(msg);
#endif
}

void ke::util::Logger::debug(const char *msg) const
{
#ifdef DEBUG
    mLogger->debug(msg);
#endif
}

void ke::util::Logger::info(const char *msg) const
{
#ifdef DEBUG
    mLogger->info(msg);
#endif
}

void ke::util::Logger::warn(const char *msg) const
{
#ifdef DEBUG
    mLogger->warn(msg);
#endif
}

void ke::util::Logger::error(const char *msg) const
{
#ifdef DEBUG
    mLogger->error(msg);
#endif
}

void ke::util::Logger::critical(const char *msg) const
{
#ifdef DEBUG
    mLogger->critical(msg);
#endif
}
