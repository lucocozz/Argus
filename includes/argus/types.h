/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_TYPES_H
#define ARGUS_TYPES_H

#define _GNU_SOURCE  // NOLINT

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Forward declarations */
typedef struct argus_s         argus_t;
typedef struct argus_option_s  argus_option_t;
typedef union argus_value_u    argus_value_t;
typedef struct argus_pair_s    argus_pair_t;
typedef union validator_data_u validator_data_t;

/**
 * argus_valtype_t - Types of values an option can hold
 */
typedef enum argus_valtype_e
{
    VALUE_TYPE_NONE = 0,

    VALUE_TYPE_INT    = 1 << 0,
    VALUE_TYPE_STRING = 1 << 1,
    VALUE_TYPE_FLOAT  = 1 << 2,
    VALUE_TYPE_BOOL   = 1 << 3,
    VALUE_TYPE_FLAG   = 1 << 4,

    VALUE_TYPE_ARRAY_STRING = 1 << 5,
    VALUE_TYPE_ARRAY_INT    = 1 << 6,
    VALUE_TYPE_ARRAY_FLOAT  = 1 << 7,

    VALUE_TYPE_MAP_STRING = 1 << 8,
    VALUE_TYPE_MAP_INT    = 1 << 9,
    VALUE_TYPE_MAP_FLOAT  = 1 << 10,
    VALUE_TYPE_MAP_BOOL   = 1 << 11,

    VALUE_TYPE_CUSTOM = 1 << 12,
} argus_valtype_t;

#define VALUE_TYPE_ANY_NUMERIC (VALUE_TYPE_INT | VALUE_TYPE_FLOAT)
#define VALUE_TYPE_ANY_BOOL    (VALUE_TYPE_BOOL | VALUE_TYPE_FLAG)
#define VALUE_TYPE_PRIMITIVE                                                                       \
    (VALUE_TYPE_INT | VALUE_TYPE_STRING | VALUE_TYPE_FLOAT | VALUE_TYPE_BOOL)
#define VALUE_TYPE_ARRAY (VALUE_TYPE_ARRAY_STRING | VALUE_TYPE_ARRAY_INT | VALUE_TYPE_ARRAY_FLOAT)
#define VALUE_TYPE_MAP                                                                             \
    (VALUE_TYPE_MAP_STRING | VALUE_TYPE_MAP_INT | VALUE_TYPE_MAP_FLOAT | VALUE_TYPE_MAP_BOOL)

/**
 * argus_optype_t - Types of command line elements
 */
typedef enum argus_optype_e
{
    TYPE_NONE = 0,
    TYPE_OPTION,     /* Standard option with - or -- prefix */
    TYPE_GROUP,      /* Logical grouping of options */
    TYPE_POSITIONAL, /* Positional argument */
    TYPE_SUBCOMMAND, /* Subcommand with its own options */
} argus_optype_t;

/**
 * argus_optflags_t - Flags that modify option behavior
 */
typedef enum argus_optflags_e
{
    FLAG_NONE = 0,
    /* Option flags */
    FLAG_REQUIRED      = 1 << 0, /* Option must be specified */
    FLAG_HIDDEN        = 1 << 1, /* Option is hidden from help */
    FLAG_ADVANCED      = 1 << 2, /* Option is marked as advanced */
    FLAG_DEPRECATED    = 1 << 3, /* Option is marked as deprecated */
    FLAG_EXPERIMENTAL  = 1 << 4, /* Option is marked as experimental */
    FLAG_EXIT          = 1 << 5, /* Program exits after option handler */
    FLAG_ENV_OVERRIDE  = 1 << 6, /* Option value can be overridden by environment variable */
    FLAG_AUTO_ENV      = 1 << 7, /* Automatically generate environment variable name */
    FLAG_NO_ENV_PREFIX = 1 << 8, /* Do not use environment variable prefix */

    /* Array an Map type flags */
    FLAG_SORTED       = 1 << 9,  /* Array values are sorted */
    FLAG_UNIQUE       = 1 << 10, /* Array values are unique */
    FLAG_SORTED_VALUE = 1 << 11, /* Map values are sorted */
    FLAG_SORTED_KEY   = 1 << 12, /* Map keys are sorted */
    FLAG_UNIQUE_VALUE = 1 << 13, /* Map values are unique */

    /* Group flags */
    FLAG_EXCLUSIVE = 1 << 14, /* Only one option in group can be set */
} argus_optflags_t;

#define FLAG_OPTIONAL (FLAG_REQUIRED ^ FLAG_REQUIRED)

/* Flag masks for validation */
#define VERSIONING_FLAG_MASK (FLAG_DEPRECATED | FLAG_EXPERIMENTAL)
#define OPTION_FLAG_MASK                                                                           \
    (FLAG_REQUIRED | FLAG_HIDDEN | FLAG_ADVANCED | FLAG_EXIT | VERSIONING_FLAG_MASK)
#define OPTION_ARRAY_FLAG_MASK (FLAG_SORTED | FLAG_UNIQUE | VERSIONING_FLAG_MASK)
#define OPTION_MAP_FLAG_MASK                                                                       \
    (FLAG_SORTED_VALUE | FLAG_SORTED_KEY | FLAG_UNIQUE_VALUE | VERSIONING_FLAG_MASK)
#define GROUP_FLAG_MASK      (FLAG_EXCLUSIVE)
#define POSITIONAL_FLAG_MASK (FLAG_REQUIRED)
#define SUBCOMMAND_FLAG_MASK (FLAG_HIDDEN | FLAG_ADVANCED | VERSIONING_FLAG_MASK)

/**
 * argus_value_u - Union to hold option values of different types
 */
union argus_value_u {
    uintptr_t raw;
    void     *as_ptr;

    char  as_char;
    char *as_string;

    int     as_int;
    int8_t  as_int8;
    int16_t as_int16;
    int32_t as_int32;
    int64_t as_int64;

    double as_float;
    bool   as_bool;

    char         **as_array_string;
    int64_t       *as_array_int;
    double        *as_array_float;
    argus_value_t *as_array; /* Generic array */
    argus_pair_t  *as_map;
};

typedef struct argus_pair_s
{
    const char   *key;
    argus_value_t value;
} argus_pair_t;

/**
 * Array iterator structure to efficiently iterate over array elements
 */
typedef struct argus_array_iterator_s
{
    argus_value_t *_array;    /* Pointer to the array */
    size_t         _count;    /* Number of elements */
    size_t         _position; /* Current position */
    argus_value_t  value;     /* Current value */
} argus_array_it_t;

/**
 * Map iterator structure to efficiently iterate over key-value pairs
 */
typedef struct argus_map_iterator_s
{
    argus_pair_t *_map;      /* Pointer to the map */
    size_t        _count;    /* Number of elements */
    size_t        _position; /* Current position */
    const char   *key;       /* Current key */
    argus_value_t value;     /* Current value */
} argus_map_it_t;

/**
 * argus_range_t - Min/max range for numeric validation
 */
typedef struct argus_range_s
{
    int64_t min;
    int64_t max;
} argus_range_t;

/**
 * regex_data_t - Data structure for regex validation
 */
typedef struct regex_data_s
{
    const char *pattern; /* Regex pattern string */
    const char *hint;    /* Details for error message and/or format explanation */
} regex_data_t;

/**
 * choices_data_t - Data structure for choice validation
 *
 * This structure holds an array of valid choices for an option.
 * It is used to validate that the provided value matches one of the choices.
 */
typedef struct choices_data_s
{
    union {
        char   **as_strings;
        int64_t *as_ints;
        double  *as_floats;
    };
    size_t          count;
    argus_valtype_t type;
} choices_data_t;

/**
 * validator_data_u - Data used by validator functions
 */
union validator_data_u {
    uintptr_t      custom;  /* Custom validator data */
    argus_range_t  range;   /* Range limits */
    regex_data_t   regex;   /* Regex pattern and info */
    choices_data_t choices; /* Choices for validation */
};

/* Callback function types */
typedef int (*argus_handler_t)(argus_t *, argus_option_t *, char *);
typedef int (*argus_free_handler_t)(argus_option_t *);
typedef int (*argus_validator_t)(argus_t *, void *, validator_data_t);
typedef char *(*argus_validator_formatter_t)(validator_data_t);
typedef int (*argus_action_t)(argus_t *, void *);

typedef enum validation_order_e
{
    ORDER_PRE = 0, /* Validation before option processing */
    ORDER_POST,    /* Validation after option processing */
} validation_order_t;

/**
 * validator_entry_t - Entry for a validator function
 */
typedef struct validator_entry_s
{
    argus_validator_t           func;
    validator_data_t            data;
    validation_order_t          order;
    argus_validator_formatter_t formatter;
} validator_entry_t;

/**
 * argus_option_s - Defines a command-line option
 */
struct argus_option_s
{
    /* Base metadata */
    argus_optype_t type;

    /* Naming metadata */
    const char *name;  /* Internal name used for references */
    char        sname; /* Single-character name (e.g., -v) */
    const char *lname; /* Long name (e.g., --verbose) */
    const char *help;  /* Help text */
    const char *hint;  /* Value hint displayed in help */

    /* Value metadata */
    argus_valtype_t value_type;
    argus_value_t   value;
    bool            is_allocated;
    argus_value_t   default_value;
    bool            have_default;
    size_t          value_count;
    size_t          value_capacity;
    char           *env_name;

    /* Callbacks metadata */
    argus_handler_t      handler;
    argus_free_handler_t free_handler;
    validator_entry_t  **validators;

    /* Dependencies metadata */
    const char **conflict;
    const char **require;

    /* Flags and state metadata */
    argus_optflags_t flags;
    bool             is_set;

    /* Subcommand metadata */
    argus_action_t         action;
    struct argus_option_s *sub_options;

    /* Context metadata */
    int   line;
    char *file;
};

#define MULTI_VALUE_INITIAL_CAPACITY 8

/* Maximum depth of nested subcommands */
#ifndef MAX_SUBCOMMAND_DEPTH
    #define MAX_SUBCOMMAND_DEPTH 8
#endif

/**
 * argus_helper_config_t - Helper configuration and theme settings
 */
typedef struct argus_helper_config_s
{
    /* Layout configuration */
    size_t max_line_width;      // Maximum width of a line
    size_t description_column;  // Column where descriptions start
    size_t option_indent;       // Indentation for options

    /* Smart Hint configuration */
    size_t smart_hint_max_length;    // Maximum length for short hints
    bool   smart_hint_allow_spaces;  // Allow spaces in short hints
} argus_helper_config_t;

/**
 * argus_helper_t - Helper configuration structure
 */
typedef struct argus_helper_s
{
    argus_helper_config_t config;
} argus_helper_t;

/**
 * argus_s - Main library context
 */
struct argus_s
{
    /* Public fields */
    const char    *program_name;
    const char    *version;
    const char    *description;
    const char    *env_prefix;
    int            error_code;
    argus_helper_t helper;

    /* Internal fields - do not access directly */
    argus_option_t       *options;
    const argus_option_t *subcommand_stack[MAX_SUBCOMMAND_DEPTH];
    size_t                subcommand_depth;
};

#endif /* ARGUS_TYPES_H */
