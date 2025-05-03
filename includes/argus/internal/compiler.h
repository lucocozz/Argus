/**
 * argus/internal/compiler.h - Compiler-specific macros and utilities
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines macros for handling compiler-specific behavior.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_COMPILER_H
#define ARGUS_INTERNAL_COMPILER_H

/**
 * Macros to disable and restore specific warnings
 */
#if defined(__clang__)
    /* For Clang compiler */
    #define PRAGMA_DISABLE_OVERRIDE()                                                              \
        _Pragma("clang diagnostic push")                                                           \
            _Pragma("clang diagnostic ignored \"-Winitializer-overrides\"")

    #define PRAGMA_DISABLE_PEDANTIC()                                                              \
        _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wpedantic\"")

    #define PRAGMA_DISABLE_VARIADIC_MACROS()                                                       \
        _Pragma("clang diagnostic push")                                                           \
            _Pragma("clang diagnostic ignored \"-Wgnu-zero-variadic-macro-arguments\"")

    #define PRAGMA_RESTORE() _Pragma("clang diagnostic pop")

#elif defined(__GNUC__)
    /* For GCC compiler */
    #define PRAGMA_DISABLE_OVERRIDE()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Woverride-init\"")

    #define PRAGMA_DISABLE_PEDANTIC()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

    #define PRAGMA_DISABLE_VARIADIC_MACROS()                                                       \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"")

    #define PRAGMA_RESTORE() _Pragma("GCC diagnostic pop")

#else
    /* Fallback for other compilers */
    #define PRAGMA_DISABLE_OVERRIDE()
    #define PRAGMA_DISABLE_PEDANTIC()
    #define PRAGMA_DISABLE_VARIADIC_MACROS()
    #define PRAGMA_RESTORE()
#endif

#endif /* ARGUS_INTERNAL_COMPILER_H */
