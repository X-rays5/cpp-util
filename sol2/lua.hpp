//
// Created by X-ray on 6/12/2021.
//
#pragma once
#include <sol/sol.hpp>

namespace lua {
    class lua_exception {
    public:
        lua_exception(std::string what) : what_(std::move(what))
        {
        }

        std::string what() {
            return what_;
        }
    private:
        std::string what_;
    };

    class lua_state {
    public:
        lua_state() {
            lua_state_.open_libraries(sol::lib::base,
                                      sol::lib::os,
                                      sol::lib::string,
                                      sol::lib::utf8,
                                      sol::lib::bit32,
                                      sol::lib::debug,
                                      sol::lib::table,
                                      sol::lib::math,
                                      sol::lib::coroutine,
                                      sol::lib::io,
                                      sol::lib::package
                                      );
            lua_state_.set_panic(sol::c_call<decltype(&lua_state::Panic), &lua_state::Panic>);
        }

        template<typename... Args>
        void RegisterFunction(std::string name, Args&&... args) {
            lua_state_.set_function(name, std::forward<Args>(args)...);
        }

        void ExcecuteFile(std::string path) {
            sol::load_result script = lua_state_.load_file(path);

            if (!script.valid()) {
                sol::error e = script;
                throw(lua_exception(e.what()));
            } else {
                ExcecuteLua(script);
            }
        }

        // This function is not safe and can cause lua to go into panic state and abort()
        void RunText(std::string text) {
            auto result = lua_state_.script(text);
            if (!result.valid()) {
                sol::error e = result;
                throw(lua_exception(e.what()));
            }
        }
    private:
        sol::state lua_state_;
    private:
        static void ExcecuteLua(sol::load_result& lua_file) {
            auto result = lua_file.call<>();
            if (!result.valid()) {
                sol::error e = result;
                throw(lua_exception(e.what()));
            }
        }

        inline void Panic(sol::optional<std::string> maybe_msg) {
            std::cerr << "Lua is in a panic state application is closing\n";
            if (maybe_msg) {
                const std::string& msg = maybe_msg.value();
                std::cerr << "\tError message: " << msg << std::endl;
            }
        }
    };
} // lua
