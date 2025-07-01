/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
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

    /* Validation errors */
    ARGUS_STRUCT_ERROR,
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
    ARGUS_ERROR_NO_VALUE,
    ARGUS_ERROR_INVALID_TYPE,
    ARGUS_ERROR_INVALID_INDEX,
    ARGUS_ERROR_INVALID_KEY,

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
 * argus_strerror - Get string description of an error code
 *
 * @param error  Error code
 * @return String description of the error
 */
const char *argus_strerror(argus_error_type_t error);

/* Forward declaration of internal function */

#include <stdarg.h>
#include <stdio.h>

static inline void argus_struct_error(argus_option_t *option, const char *fmt, ...)
{
    fprintf(stderr, "%s:%d -> ", option->file, option->line);
    switch (option->type) {
        case TYPE_OPTION:
            fprintf(stderr, "Option [ ");
            break;
        case TYPE_GROUP:
            fprintf(stderr, "Group [ ");
            break;
        case TYPE_POSITIONAL:
            fprintf(stderr, "Positional [ ");
            break;
        case TYPE_SUBCOMMAND:
            fprintf(stderr, "Subcommand [ ");
            break;
        case TYPE_NONE:
            fprintf(stderr, "Unknown [ ");
            break;
        default:
            break;
    }
    if (option->name == NULL)
        fprintf(stderr, "unnamed");
    else if (option->type == TYPE_OPTION) {
        if (option->lname)
            fprintf(stderr, "--%s", option->lname);
        if (option->sname)
            fprintf(stderr, " -%c", option->sname);
    } else
        fprintf(stderr, "%s", option->name);
    fprintf(stderr, " ]:\n\t");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

static inline void argus_parsing_error(argus_t *argus, argus_error_type_t error_type,
                                       const char *fmt, ...)
{
    va_list args;

    argus->error_code = (int)error_type;
    fprintf(stderr, "%s: ", argus->program_name);

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fprintf(stderr, "\n");
}

/**
 * ARGUS_STRUCT_ERROR - Report a structure error
 */
#define ARGUS_STRUCT_ERROR(option, ...) argus_struct_error(option, __VA_ARGS__)

/**
 * ARGUS_PARSING_ERROR - Report a parsing error
 */
#define ARGUS_PARSING_ERROR(argus, error_type, ...)                                                \
    argus_parsing_error(argus, error_type, __VA_ARGS__)

#endif /* ARGUS_ERRORS_H */
