#include "dbg_log.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef GAME_DEBUG
    fn _dbg_log_function(cstring fmt, ...) -> void {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        printf("\n");
    }
#endif