/**
 * argus/internal/platform/string_compat.h - String function compatibility utilities
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header provides cross-platform compatibility for string functions
 * that have different implementations or naming conventions across platforms.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_CROSS_PLATFORM_H
#define ARGUS_INTERNAL_CROSS_PLATFORM_H

#include <stdlib.h>
#include <string.h>

/**
 * String function compatibility definitions
 */
#if defined(_MSC_VER)

    #define safe_strdup _strdup

static inline char *safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    strcpy_s(dest, dest_size, src);
    return dest;
}

static inline char *safe_strncpy(char *dest, size_t dest_size, const char *src, size_t count)
{
    strncpy_s(dest, dest_size, src, count);
    return dest;
}

static inline char *safe_strcat(char *dest, size_t dest_size, const char *src)
{
    strcat_s(dest, dest_size, src);
    return dest;
}

static inline char *safe_strncat(char *dest, size_t dest_size, const char *src, size_t count)
{
    strncat_s(dest, dest_size, src, count);
    return dest;
}

static inline char *safe_strndup(const char *s, size_t n)
{
    size_t len     = strnlen(s, n);
    char  *new_str = (char *)malloc(len + 1);

    if (new_str == NULL) {
        return NULL;
    }

    memcpy(new_str, s, len);
    new_str[len] = '\0';
    return new_str;
}

#else
    #define safe_strdup strdup

static inline char *safe_strcpy(char *dest, size_t dest_size, const char *src)
{
    (void)dest_size;
    return strcpy(dest, src);
}

static inline char *safe_strncpy(char *dest, size_t dest_size, const char *src, size_t count)
{
    (void)dest_size;
    return strncpy(dest, src, count);
}

static inline char *safe_strcat(char *dest, size_t dest_size, const char *src)
{
    (void)dest_size;
    return strcat(dest, src);
}

static inline char *safe_strncat(char *dest, size_t dest_size, const char *src, size_t count)
{
    (void)dest_size;
    return strncat(dest, src, count);
}

static inline char *safe_strndup(const char *s, size_t n)
{
    // #ifdef __GLIBC__
    // return strndup(s, n);
    // #else

    size_t len = 0;
    while (len < n && s[len] != '\0')
        len++;

    char *new_str = (char *)malloc(len + 1);
    if (new_str == NULL)
        return NULL;

    memcpy(new_str, s, len);
    new_str[len] = '\0';
    return new_str;
    // #endif
}
#endif

#endif /* ARGUS_INTERNAL_CROSS_PLATFORM_H */
