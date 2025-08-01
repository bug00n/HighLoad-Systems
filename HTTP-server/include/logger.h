#pragma once

#include <memory>
#include <string>
#include <spdlog/spdlog.h>

enum Level {
    Debug,
    Info,
    Warning,
    Error,
    Critical,
    Off
};

class Logger {
 private:
    static std::shared_ptr<spdlog::logger> logger;

    static std::shared_ptr<spdlog::logger>& get();

 public:
    static void init(const std::string log_path = "logs/log.txt", const Level log_level = Level::Info);

    static spdlog::level::level_enum level_cast(const Level log_level);

    template<typename... Args>
    static void debug(fmt::format_string<Args...> fmt, const Args&... args) {
        if (logger && logger->should_log(spdlog::level::debug))
            logger->debug(fmt, args...);
    }

    template<typename... Args>
    static void info(fmt::format_string<Args...> fmt, Args&&... args) {
        get()->info(fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(fmt::format_string<Args...> fmt, const Args&... args) {
        if (logger && logger->should_log(spdlog::level::warn))
            logger->warn(fmt, args...);
    }

    template<typename... Args>
    static void error(fmt::format_string<Args...> fmt, const Args&... args) {
        if (logger && logger->should_log(spdlog::level::err))
            logger->error(fmt, args...);
    }

    template<typename... Args>
    static void critical(fmt::format_string<Args...> fmt, const Args&... args) {
        if (logger && logger->should_log(spdlog::level::critical))
            logger->critical(fmt, args...);
    }
};
