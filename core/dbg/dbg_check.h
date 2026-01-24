#pragma once

#include "dbg_log.h"

// @Note: Debug break macro implementation.

#ifdef GAME_DEBUG
#   ifdef GAME_WIN
#       define dbg_break() __debugbreak()
#   else
#       error "Unsupported OS!"
#   endif
#else
#   define dbg_break()
#endif

// @Note: Assert macro implementation.

#ifdef GAME_DEBUG
    template<typename T, typename... TArgs>
    fn _dbg_check_function(T&& expr, cstring fmt, TArgs&&... args) {
        if (!expr) {
            _dbg_log_function(fmt, std::forward<TArgs>(args)...);
            dbg_break();
        }
    }
    template<typename T, typename... TArgs>
    fn _dbg_ensure_function(T&& expr, cstring fmt, TArgs&&... args) -> T&& {
        _dbg_check_function(std::forward<T>(expr), fmt, std::forward<T>(args)...);
        return std::forward<T>(expr);
    }
#endif

#ifdef GAME_DEBUG
#   define dbg_check(X, ...) (_dbg_check_function(X, __VA_ARGS__))
#   define dbg_ensure(X, ...) (_dbg_ensure_function(X, __VA_ARGS__))
#else
#   define dbg_check(...)
#   define dbg_ensure(X, ...) (X)
#endif