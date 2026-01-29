#pragma once

#include "base_log.h"

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
    fn _checkf_function(T&& expr, cstring fmt, TArgs&&... args) {
        if (!expr) {
            _logf_function(fmt, std::forward<TArgs>(args)...);
            dbg_break();
        }
    }
    template<typename T, typename... TArgs>
    fn _ensuref_function(T&& expr, cstring fmt, TArgs&&... args) -> T&& {
        _checkf_function(std::forward<T>(expr), fmt, std::forward<T>(args)...);
        return std::forward<T>(expr);
    }
#endif

#ifdef GAME_DEBUG
#   define checkf(X, ...) (_checkf_function(X, __VA_ARGS__))
#   define ensuref(X, ...) (_ensuref_function(X, __VA_ARGS__))
#else
#   define checkf(...)
#   define ensuref(X, ...) (X)
#endif