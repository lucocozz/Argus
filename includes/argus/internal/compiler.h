/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_INTERNAL_COMPILER_H
#define ARGUS_INTERNAL_COMPILER_H

/**
 * Macros to disable and restore specific warnings
 */
#if defined(__clang__)
    /* For Clang compiler */
    #define ARGUS_COMPILER_RULE_PUSH()                                                             \
        _Pragma("clang diagnostic push")                                                           \
            _Pragma("clang diagnostic ignored \"-Wgnu-zero-variadic-macro-arguments\"")            \
                _Pragma("clang diagnostic ignored \"-Winitializer-overrides\"")                    \
                    _Pragma("clang diagnostic ignored \"-Wpedantic\"")

    #define ARGUS_COMPILER_RULE_POP() _Pragma("clang diagnostic pop")

#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
    /* For GCC compiler and MinGW variants */

    #if defined(__WIN32__) || defined(__WIN64__) || defined(__CYGWIN__)
        #define ARGUS_COMPILER_RULE_PUSH()                                                         \
            _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"") \
                _Pragma("GCC diagnostic ignored \"-Woverride-init\"")                              \
                    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #else
        #define ARGUS_COMPILER_RULE_PUSH()                                                         \
            _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"") \
                _Pragma("GCC diagnostic ignored \"-Woverride-init\"")                              \
                    _Pragma("GCC diagnostic ignored \"-Wpedantic\"")
    #endif

    #define ARGUS_COMPILER_RULE_POP() _Pragma("GCC diagnostic pop")

#else
    /* Fallback for other compilers */
    #define ARGUS_COMPILER_RULE_PUSH()
    #define ARGUS_COMPILER_RULE_POP()
#endif

/**
 * Shared library export/import macros for Windows DLL support
 */
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__) ||           \
    defined(__MINGW64__)
    #if defined(ARGUS_BUILDING)
        #if defined(_MSC_VER)
            #define ARGUS_API __declspec(dllexport)
        #else  // MinGW/GCC
            #define ARGUS_API __declspec(dllexport) __cdecl
        #endif
    #else
        #if defined(_MSC_VER)
            #define ARGUS_API __declspec(dllimport)
        #else  // MinGW/GCC
            #define ARGUS_API __declspec(dllimport) __cdecl
        #endif
    #endif
#else
    #define ARGUS_API
#endif

#endif /* ARGUS_INTERNAL_COMPILER_H */
