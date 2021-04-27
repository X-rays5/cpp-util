//
// Created by X-ray on 3/24/2021.
//
#pragma once
#include "spdlogwrapper.hpp"

namespace sdk {
    // the checks for ndebug prevent the library to start logging in release mode
    void Logger::loggerInit() {
#ifndef NDEBUG
        SetConsoleTitleA("program name");
        spdlog::set_pattern("[%H:%M:%S] [%^%l%$] [%n] [thread %t]: %v");
        spdlog::set_level(spdlog::level::debug);
        spdlog::flush_every(std::chrono::seconds(5));
        g_Logger->consoleLog_ = spdlog::stdout_color_mt("program name Console log");

        if (std::filesystem::exists("debug.log"))
            std::filesystem::remove("debug.log");

        this->fileLog_ = spdlog::basic_logger_mt<spdlog::async_factory>("program name File log", "debug.log");
#endif
    }

    void Logger::log(logtype type, std::string text) {
#ifndef NDEBUG
        switch(type) {
            case Logger::info: {
                g_Logger->consoleLog_->info(text);
                g_Logger->fileLog_->info(text);
                break;
            }
            case Logger::warn: {
                g_Logger->consoleLog_->warn(text);
                g_Logger->fileLog_->warn(text);
                break;
            }
            case Logger::critical: {
                g_Logger->consoleLog_->critical(text);
                g_Logger->fileLog_->critical(text);
                break;
            }
            case Logger::error: {
                g_Logger->consoleLog_->error(text);
                g_Logger->fileLog_->error(text);
                break;
            }
            case Logger::debug: {
                g_Logger->consoleLog_->debug(text);
                g_Logger->fileLog_->debug(text);
                break;
            }
        }
#endif
    }
}