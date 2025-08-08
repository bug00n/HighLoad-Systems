#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Logger.h"

std::shared_ptr<spdlog::logger> Logger::logger = nullptr;

std::shared_ptr<spdlog::logger>& Logger::get() {
    return logger;
}

void Logger::init(const std::string log_path, const Level log_level) {
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern("[%Y-%m-%d][%H:%M:%S][%l][%n] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_path, false);
    file_sink->set_pattern("[%Y-%m-%d][%H:%M:%S][%l][%n] %v");

    std::vector<spdlog::sink_ptr> sinks {console_sink, file_sink};

    logger = std::make_shared<spdlog::logger>("main_logger", sinks.begin(), sinks.end());
    logger->set_level(level_cast(log_level));

    spdlog::set_default_logger(logger);

    Logger::debug("The logger has been created");
}

spdlog::level::level_enum Logger::level_cast(const Level log_level) {
    switch (log_level) {
        case Level::Debug:    return spdlog::level::debug;
        case Level::Info:     return spdlog::level::info;
        case Level::Warning:  return spdlog::level::warn;
        case Level::Error:    return spdlog::level::err;
        case Level::Critical: return spdlog::level::critical;
        case Level::Off:      return spdlog::level::off;
    }
    return spdlog::level::info;
}
