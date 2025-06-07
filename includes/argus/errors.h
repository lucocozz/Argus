/**
 * argus/errors.h - Error handling
 *
 * This header defines error codes and error handling functions
 * that are part of the public API.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_ERRORS_H
#define ARGUS_ERRORS_H

#include "argus/types.h"
#include <stddef.h>

/**
 * Error codes returned by argus functions
 */
typedef enum argus_error_type_e
{
    ARGUS_SUCCESS = 0,
    ARGUS_SHOULD_EXIT,

    /* Structure errors */
    ARGUS_ERROR_DUPLICATE_OPTION,
    ARGUS_ERROR_INVALID_HANDLER,
    ARGUS_ERROR_INVALID_DEFAULT,
    ARGUS_ERROR_INVALID_GROUP,
    ARGUS_ERROR_INVALID_DEPENDENCY,
    ARGUS_ERROR_INVALID_FLAG,
    ARGUS_ERROR_INVALID_POSITION,
    ARGUS_ERROR_MALFORMED_OPTION,
    ARGUS_ERROR_MISSING_HELP,

    /* Parsing errors */
    ARGUS_ERROR_INVALID_ARGUMENT,
    ARGUS_ERROR_MISSING_VALUE,
    ARGUS_ERROR_MISSING_REQUIRED,
    ARGUS_ERROR_CONFLICTING_OPTIONS,
    ARGUS_ERROR_INVALID_FORMAT,
    ARGUS_ERROR_EXCLUSIVE_GROUP,
    ARGUS_ERROR_INVALID_CHOICE,
    ARGUS_ERROR_INVALID_RANGE,
    ARGUS_ERROR_AMBIGUOUS_SUBCOMMAND,

    /* Execution errors */
    ARGUS_ERROR_NO_COMMAND,

    /* Internal errors */
    ARGUS_ERROR_MEMORY,
    ARGUS_ERROR_INTERNAL,
    ARGUS_ERROR_UNSUPPORTED,

    /* Value errors */
    ARGUS_ERROR_INVALID_VALUE,

    /* Stack errors */
    ARGUS_ERROR_STACK_OVERFLOW,
} argus_error_type_t;

/**
 * argus_print_error_stack - Print all errors in the error stack
 *
 * @param argus  Argus context
 */
void argus_print_error_stack(const argus_t *argus);

/**
 * argus_strerror - Get string description of an error code
 *
 * @param error  Error code
 * @return String description of the error
 */
const char *argus_strerror(argus_error_type_t error);

/* Forward declaration of internal function */
argus_error_context_t get_error_context(argus_t *argus);
void                  argus_push_error(argus_t *argus, argus_error_t error);

#include <stdarg.h>
#include <stdio.h>

#define ARGUS_OK() ((argus_error_t){.code = ARGUS_SUCCESS})

/* Functions that implement the actual error handling logic */
static inline void argus_collect_error(argus_t *argus, int code, const char *fmt, ...)
{
    argus_error_t error;
    va_list       args;

    error.code    = code;
    error.context = get_error_context(argus);

    va_start(args, fmt);
    vsnprintf(error.message, ARGUS_MAX_ERROR_MESSAGE_SIZE, fmt, args);
    va_end(args);

    argus_push_error(argus, error);
}

static inline void argus_report_error(argus_t *argus, const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "%s: ", argus->program_name);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
    argus->error_stack.count++;
}

/**
 * ARGUS_COLLECT_ERROR - Collect an error in the error stack
 * This version uses an inline function to handle the variadic arguments correctly
 */
#define ARGUS_COLLECT_ERROR(argus, code, ...) argus_collect_error(argus, code, __VA_ARGS__)

/**
 * ARGUS_REPORT_ERROR - Report an error and return
 * This version uses an inline function to handle the variadic arguments correctly
 */
#define ARGUS_REPORT_ERROR(argus, code, ...)                                                       \
    do {                                                                                           \
        argus_report_error(argus, __VA_ARGS__);                                                    \
        return (code);                                                                             \
    } while (0)

#endif /* ARGUS_ERRORS_H */
