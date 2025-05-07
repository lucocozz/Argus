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
 * Platform detection macros
 */
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__) ||           \
    defined(__MINGW64__)
    #define ARGUS_PLATFORM_WINDOWS 1
#else
    #define ARGUS_PLATFORM_WINDOWS 0
#endif

/**
 * Compiler detection
 */
#if defined(_MSC_VER)
    #define ARGUS_COMPILER_MSVC 1
#else
    #define ARGUS_COMPILER_MSVC 0
#endif

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

#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
    /* For GCC compiler and MinGW variants */
    #define PRAGMA_DISABLE_OVERRIDE()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Woverride-init\"")

    #define PRAGMA_DISABLE_PEDANTIC()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

    #define PRAGMA_DISABLE_VARIADIC_MACROS()                                                       \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"")

    #define PRAGMA_RESTORE() _Pragma("GCC diagnostic pop")

#elif defined(_MSC_VER)
    /* For Microsoft Visual C++ compiler */
    #define PRAGMA_DISABLE_OVERRIDE()                                                              \
        __pragma(warning(push))                                                                    \
            __pragma(warning(disable : 4028)) /* formal parameter different from declaration */    \
            __pragma(warning(disable : 4133)) /* incompatible types */

    #define PRAGMA_DISABLE_PEDANTIC()                                                              \
        __pragma(warning(push))                                                                    \
            __pragma(warning(disable : 4204)) /* non-constant aggregate initializer */

    #define PRAGMA_DISABLE_VARIADIC_MACROS() __pragma(warning(push))

    #define PRAGMA_RESTORE() __pragma(warning(pop))

#else
    /* Fallback for other compilers */
    #define PRAGMA_DISABLE_OVERRIDE()
    #define PRAGMA_DISABLE_PEDANTIC()
    #define PRAGMA_DISABLE_VARIADIC_MACROS()
    #define PRAGMA_RESTORE()
#endif

/**
 * Shared library export/import macros for Windows DLL support
 */
#if ARGUS_PLATFORM_WINDOWS
    #if ARGUS_COMPILER_MSVC
        /* MSVC specific export/import macros */
        #if defined(ARGUS_BUILD_SHARED)
            #define ARGUS_API __declspec(dllexport)
        #elif defined(ARGUS_USE_SHARED)
            #define ARGUS_API __declspec(dllimport)
        #else
            #define ARGUS_API
        #endif
    #else
        /* MinGW specific export/import macros with __cdecl calling convention */
        #if defined(ARGUS_BUILD_SHARED)
            #define ARGUS_API __declspec(dllexport) __cdecl
        #elif defined(ARGUS_USE_SHARED)
            #define ARGUS_API __declspec(dllimport) __cdecl
        #else
            #define ARGUS_API __cdecl
        #endif
    #endif
#else
    #define ARGUS_API
#endif

/**
 * Additional MSVC-specific macros
 */
#if ARGUS_COMPILER_MSVC
    /* Disable specific MSVC warnings that might occur in the code */
    #pragma warning(disable : 4996) /* Function or variable may be unsafe */
    #pragma warning(disable : 4244) /* Possible loss of data in conversion */

    /* Add C99 compatibility for MSVC */
    #if _MSC_VER < 1900 /* Before Visual Studio 2015 */
        #define inline   __inline
        #define snprintf _snprintf
    #endif

    /* MSVC doesn't support C99 variadic macros the same way as GCC/Clang */
    #define ARGUS_VA_ARGS(...) __VA_ARGS__
#else
    #define ARGUS_VA_ARGS(...) __VA_ARGS__
#endif

#endif /* ARGUS_INTERNAL_COMPILER_H */
