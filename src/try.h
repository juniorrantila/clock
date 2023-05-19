#pragma once

#define TRY(__expr)                                 \
    ({                                              \
        __auto_type __temporary_result = (__expr);  \
        if (__temporary_result == -1)               \
            return -1;                              \
        __temporary_result;                         \
    })

void perror(const char *);
#define MUST(__expr)                                \
    ({                                              \
        __auto_type __temporary_result = (__expr);  \
        if (__temporary_result == -1) {             \
            perror(#__expr);                        \
            __builtin_trap();                       \
        }                                           \
        __temporary_result;                         \
    })
