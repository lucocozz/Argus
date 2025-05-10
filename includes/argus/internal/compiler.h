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

#elif defined(__GNUC__) || defined(__MINGW32__) || defined(__MINGW64__)
    /* For GCC compiler and MinGW variants */
    #define PRAGMA_DISABLE_OVERRIDE()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Woverride-init\"")

    #define PRAGMA_DISABLE_PEDANTIC()                                                              \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wpedantic\"")

    #define PRAGMA_DISABLE_VARIADIC_MACROS()                                                       \
        _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Wvariadic-macros\"")

    #define PRAGMA_RESTORE() _Pragma("GCC diagnostic pop")

// #elif defined(_MSC_VER)
//     /* For Microsoft Visual C++ compiler */
//     /* Disable problematic warnings for designated initializers and structure initialization*/
//     #define PRAGMA_DISABLE_OVERRIDE()                                                              \
//         __pragma(warning(push))                                                                    \
//         __pragma(warning(disable: 4204)) /* nonstandard extension: non-constant aggregate initializer */ \
//         __pragma(warning(disable: 4047)) /* differs in levels of indirection */                    \
//         __pragma(warning(disable: 4113)) /* differs in parameter lists */                          \
//         __pragma(warning(disable: 4057)) /* differs in indirection to slightly different base types */ \
//         __pragma(warning(disable: 4100)) /* unreferenced formal parameter */                       \
//         __pragma(warning(disable: 4702)) /* unreachable code */                                    \
//         __pragma(warning(disable: 4127)) /* conditional expression is constant */                  \
//         __pragma(warning(disable: 4701)) /* potentially uninitialized local variable */            \
//         __pragma(warning(disable: 4133)) /* incompatible types */                                  \
//         __pragma(warning(disable: 4143)) /* modifier mismatch */                                   \
//         __pragma(warning(disable: 4028)) /* formal parameter different from declaration */         \
//         __pragma(warning(disable: 4115)) /* named type definition in parentheses */                \
//         __pragma(warning(disable: 4152)) /* function/data pointer conversion in expression */      \

//     #define PRAGMA_DISABLE_PEDANTIC()                                                              \
//         __pragma(warning(push))                                                                    \
//         __pragma(warning(disable: 4204)) /* nonstandard extension: non-constant aggregate initializer */

//     #define PRAGMA_DISABLE_VARIADIC_MACROS()                                                       \
//         __pragma(warning(push))                                                                    \
//         __pragma(warning(disable: 4127)) /* conditional expression is constant */

//     #define PRAGMA_RESTORE() __pragma(warning(pop))

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

/* Internal macro to disable MSVC warnings for structure initializers */
// #if defined(_MSC_VER)
// #define ARGUS_MSVC_WARNINGS_PUSH() \
//     __pragma(warning(push)) \
//     __pragma(warning(disable: 4204)) /* nonstandard extension: non-constant aggregate initializer */ \
//     __pragma(warning(disable: 4047)) /* differs in levels of indirection */ \
//     __pragma(warning(disable: 4133)) /* incompatible types */ \
//     __pragma(warning(disable: 4113)) /* differs in parameter lists */

// #define ARGUS_MSVC_WARNINGS_POP() \
//     __pragma(warning(pop))
// #else
// #define ARGUS_MSVC_WARNINGS_PUSH()
// #define ARGUS_MSVC_WARNINGS_POP()
// #endif

#endif /* ARGUS_INTERNAL_COMPILER_H */
