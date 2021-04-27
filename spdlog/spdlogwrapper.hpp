//
// Created by X-ray on 3/16/2021.
//
#pragma once
#include <memory>
#include <filesystem>
#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/spdlog.h>

namespace sdk {
    class Logger {
    public:
        enum logtype : int {
            info,
            warn,
            critical,
            error,
            debug,
        };

        void loggerInit();
        void log(logtype type, std::string text);
    private:
        std::shared_ptr<spdlog::logger> consoleLog_;
        std::shared_ptr<spdlog::logger> fileLog_;
    };
    inline std::unique_ptr<Logger> g_Logger = nullptr;
}


#define LOG_INFO(text) if (sdk::g_Logger != nullptr) {sdk::g_Logger->log(sdk::Logger::info, text);}
#define LOG_WARN(text) if (sdk::g_Logger != nullptr) {sdk::g_Logger->log(sdk::Logger::warn, text);}
#define LOG_CRIT(text) if (sdk::g_Logger != nullptr) {sdk::g_Logger->log(sdk::Logger::critical, text);}
#define LOG_ERR(text) if (sdk::g_Logger != nullptr) {sdk::g_Logger->log(sdk::Logger::error, text);}
#define LOG_DBG(text) if (sdk::g_Logger != nullptr) {sdk::g_Logger->log(sdk::Logger::debug, text);}