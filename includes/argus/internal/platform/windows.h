/**
 * argus/internal/platform/windows.h - Windows compatibility utilities
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header provides compatibility functions and definitions for Windows.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_PLATFORM_WINDOWS_H
#define ARGUS_INTERNAL_PLATFORM_WINDOWS_H

#include "argus/internal/compiler.h"
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

#endif /* ARGUS_PLATFORM_WINDOWS */

#endif /* ARGUS_INTERNAL_PLATFORM_WINDOWS_H */
