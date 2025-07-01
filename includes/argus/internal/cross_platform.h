/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
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
    if (dest == NULL || src == NULL || dest_size == 0)
        return NULL;

    size_t src_len = strlen(src);
    if (src_len >= dest_size)
        return NULL;  // Buffer too small

    return strcpy(dest, src);
}

static inline char *safe_strncpy(char *dest, size_t dest_size, const char *src, size_t count)
{
    if (dest == NULL || src == NULL || dest_size == 0)
        return NULL;

    size_t copy_len = (count < dest_size - 1) ? count : dest_size - 1;
    strncpy(dest, src, copy_len);
    dest[copy_len] = '\0';  // Ensure null termination

    return dest;
}

static inline char *safe_strcat(char *dest, size_t dest_size, const char *src)
{
    if (dest == NULL || src == NULL || dest_size == 0)
        return NULL;

    size_t dest_len = strlen(dest);
    size_t src_len  = strlen(src);

    if (dest_len + src_len >= dest_size)
        return NULL;  // Buffer too small

    return strcat(dest, src);
}

static inline char *safe_strncat(char *dest, size_t dest_size, const char *src, size_t count)
{
    if (dest == NULL || src == NULL || dest_size == 0)
        return NULL;

    size_t dest_len  = strlen(dest);
    size_t available = dest_size - dest_len - 1;  // -1 for null terminator

    if (available == 0)
        return NULL;  // No space available

    size_t copy_len = (count < available) ? count : available;
    strncat(dest, src, copy_len);

    return dest;
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
