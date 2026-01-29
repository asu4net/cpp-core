#pragma once

// @Note: Simple log macro implementation.

#ifdef GAME_DEBUG
    fn _logf_function(cstring fmt, ...) -> void;
#endif

#ifdef GAME_DEBUG
#   define logf(X, ...) (_logf_function(X, __VA_ARGS__))
#else
#   define logf(X, ...)
#endif