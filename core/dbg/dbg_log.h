#pragma once

// @Note: Simple log macro implementation.

#ifdef GAME_DEBUG
    fn _dbg_log_function(cstring fmt, ...) -> void;
#endif

#ifdef GAME_DEBUG
#   define dbg_log(X, ...) (_dbg_log_function(X, __VA_ARGS__))
#else
#   define dbg_log(X, ...)
#endif