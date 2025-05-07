/**
 * argus/internal/windows_compat.h - Windows compatibility utilities
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header provides compatibility functions and definitions for Windows.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_WINDOWS_COMPAT_H
#define ARGUS_INTERNAL_WINDOWS_COMPAT_H

#include "compiler.h"
#include <stdlib.h>
#include <string.h>

#if ARGUS_PLATFORM_WINDOWS

/**
 * Implementation of strndup for Windows where this function is not available
 */
static inline char *argus_strndup(const char *s, size_t n)
{
    /* Use strnlen which is available on Windows */
    size_t len     = strnlen(s, n);
    char  *new_str = (char *)malloc(len + 1);

    if (new_str == NULL) {
        return NULL;
    }

    memcpy(new_str, s, len);
    new_str[len] = '\0';
    return new_str;
}

    #ifndef strndup
        #define strndup argus_strndup
    #endif

    /**
     * Safer integer type conversion macros for Windows
     */
    #if ARGUS_COMPILER_MSVC
        /* For size_t to int conversion, ensure we don't lose data */
        #define ARGUS_SIZE_TO_INT(size_val) ((size_val) > INT_MAX ? INT_MAX : (int)(size_val))

        /* For int64_t to int conversion */
        #define ARGUS_INT64_TO_INT(int64_val)                                                      \
            ((int64_val) > INT_MAX ? INT_MAX : ((int64_val) < INT_MIN ? INT_MIN : (int)(int64_val)))
    #else
        /* For GCC/Clang, simple cast is sufficient with proper warnings disabled */
        #define ARGUS_SIZE_TO_INT(size_val)   ((int)(size_val))
        #define ARGUS_INT64_TO_INT(int64_val) ((int)(int64_val))
    #endif

#endif /* ARGUS_PLATFORM_WINDOWS */

#endif /* ARGUS_INTERNAL_WINDOWS_COMPAT_H */
