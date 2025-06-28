/**
 * argus/options.h - Option definition macros
 *
 * This header provides macros for defining command-line options.
 * These are the primary interface users interact with to specify
 * what options their program accepts.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */
#ifndef ARGUS_OPTIONS_H
#define ARGUS_OPTIONS_H

#include "argus/internal/compiler.h"
#include "argus/types.h"

/* Forward declaration for handler functions */
ARGUS_API int flag_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int bool_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int string_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int int_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int float_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int help_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int version_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int default_free(argus_option_t *option);

ARGUS_API int array_string_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int array_int_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int array_float_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int free_array_string_handler(argus_option_t *option);

ARGUS_API int map_string_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int map_int_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int map_float_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int map_bool_handler(argus_t *argus, argus_option_t *option, char *value);
ARGUS_API int free_map_string_handler(argus_option_t *option);
ARGUS_API int free_map_int_handler(argus_option_t *option);
ARGUS_API int free_map_float_handler(argus_option_t *option);
ARGUS_API int free_map_bool_handler(argus_option_t *option);

ARGUS_API int range_validator(argus_t *argus, void *option_ptr, validator_data_t data);
ARGUS_API int length_validator(argus_t *argus, void *option_ptr, validator_data_t data);
ARGUS_API int count_validator(argus_t *argus, void *option_ptr, validator_data_t data);
ARGUS_API int regex_validator(argus_t *argus, void *value_ptr, validator_data_t data);
ARGUS_API int choices_string_validator(argus_t *argus, void *option_ptr, validator_data_t data);
ARGUS_API int choices_int_validator(argus_t *argus, void *option_ptr, validator_data_t data);
ARGUS_API int choices_float_validator(argus_t *argus, void *option_ptr, validator_data_t data);

ARGUS_API char *format_range_validator(validator_data_t data);
ARGUS_API char *format_length_validator(validator_data_t data);
ARGUS_API char *format_count_validator(validator_data_t data);
ARGUS_API char *format_regex_validator(validator_data_t data);
ARGUS_API char *format_choices_validator(validator_data_t data);

/*
 * Support macro for character to string conversion
 */
#define CHAR_TO_STRING(c) ((char[]){c, '\0'})

// clang-format off

/*
 * Optional option fields macros
 */
#define DEBUG_INFO()            .line = __LINE__, .file = __FILE__
#define DEFINE_NAME(lname, sname) ((lname) ? (lname) : CHAR_TO_STRING(sname))
#define DEFAULT(val)            .value = (argus_value_t){ .raw = (uintptr_t)(val) },         \
                                .default_value = (argus_value_t){ .raw = (uintptr_t)(val) }, \
                                .is_set = true, \
                                .have_default = true
#define HANDLER(fn)             .handler = (argus_handler_t)(fn)
#define ACTION(fn)              .action = (argus_action_t)(fn)
#define FREE_HANDLER(fn)        .free_handler = (argus_free_handler_t)(fn)
#define HINT(_hint)             .hint = _hint
#define REQUIRE(...)            .require = (const char*[]){__VA_ARGS__, NULL}
#define CONFLICT(...)           .conflict = (const char*[]){__VA_ARGS__, NULL}
#define GROUP_DESC(desc)        .help = desc
#define HELP(desc)              .help = desc
#define FLAGS(_flags)           .flags = _flags
#define ENV_VAR(name)           .env_name = name
#define VALIDATOR(...)          .validators = (validator_entry_t*[]){__VA_ARGS__, NULL}

/*
 * Validator macros
 */
#define MAKE_VALIDATOR(fn, _formatter_, _data_, _order_) \
    &(validator_entry_t){ \
        .func = fn, \
        .formatter = _formatter_, \
        .data = _data_, \
        .order = _order_ \
    }
#define _V_DATA_RANGE_(_min_, _max_) \
    ((validator_data_t){ .range = (argus_range_t){ .min = _min_, .max = _max_ } })
#define _V_DATA_CUSTOM_(data) \
    ((validator_data_t){ .custom = (uintptr_t)(data) })

#define V_RANGE(_min_, _max_) \
    MAKE_VALIDATOR(range_validator, format_range_validator, _V_DATA_RANGE_(_min_, _max_), ORDER_POST)
#define V_LENGTH(_min_, _max_) \
    MAKE_VALIDATOR(length_validator, format_length_validator, _V_DATA_RANGE_(_min_, _max_), ORDER_POST)
#define V_COUNT(_min_, _max_) \
    MAKE_VALIDATOR(count_validator, format_count_validator, _V_DATA_RANGE_(_min_, _max_), ORDER_POST)
#define V_REGEX(re) \
    MAKE_VALIDATOR(regex_validator, format_regex_validator, ((validator_data_t){ .regex = (re) }), ORDER_PRE)

#define V_CHOICE_STR(...) \
    MAKE_VALIDATOR(choices_string_validator, format_choices_validator, \
        ((validator_data_t){ .choices = { \
            .as_strings = (char*[]){ __VA_ARGS__ }, \
            .count = sizeof((char*[]){ __VA_ARGS__ }) / sizeof(char*), \
            .type = VALUE_TYPE_STRING \
        }}), ORDER_POST)

#define V_CHOICE_INT(...) \
    MAKE_VALIDATOR(choices_int_validator, format_choices_validator, \
        ((validator_data_t){ .choices = { \
            .as_ints = (int64_t[]){ __VA_ARGS__ }, \
            .count = sizeof((int64_t[]){ __VA_ARGS__ }) / sizeof(int64_t), \
            .type = VALUE_TYPE_INT \
        }}), ORDER_POST)

#define V_CHOICE_FLOAT(...) \
    MAKE_VALIDATOR(choices_float_validator, format_choices_validator, \
        ((validator_data_t){ .choices = { \
            .as_floats = (double[]){ __VA_ARGS__ }, \
            .count = sizeof((double[]){ __VA_ARGS__ }) / sizeof(double), \
            .type = VALUE_TYPE_FLOAT \
        }}), ORDER_POST)

/*
 * Base option definition macros
 */
#define OPTION_END() (argus_option_t){ \
    .type = TYPE_NONE, \
    .name = NULL, \
    .value_type = VALUE_TYPE_NONE, \
    DEBUG_INFO() \
}

#define OPTION_BASE(_short, _long, _value_type, ...)                                          \
    (argus_option_t) {                                                                        \
        .type = TYPE_OPTION, .name = DEFINE_NAME(_long, _short),                              \
        .sname = _short, .lname = _long, .value_type = _value_type,                           \
        .free_handler = default_free, DEBUG_INFO(), ##__VA_ARGS__                             \
    }

#define POSITIONAL_BASE(_name, _value_type, ...)                                               \
    (argus_option_t) {                                                                         \
        .type = TYPE_POSITIONAL, .name = _name, .value_type = _value_type,                     \
        .free_handler = default_free, .flags = FLAG_REQUIRED, DEBUG_INFO(), ##__VA_ARGS__     \
    }

#define GROUP_BASE(_name, ...)                                                                 \
    (argus_option_t) {                                                                         \
        .type = TYPE_GROUP, .name = _name, DEBUG_INFO(), ##__VA_ARGS__                        \
    }

#define SUBCOMMAND_BASE(_name, sub_opts, ...)                                                  \
    (argus_option_t) {                                                                         \
        .type = TYPE_SUBCOMMAND, .name = _name, .sub_options = sub_opts, DEBUG_INFO(), ##__VA_ARGS__  \
    }

// clang-format on
/*
 * Option type macros
 */
#define OPTION_FLAG(short_name, long_name, ...)                                                    \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_FLAG, HANDLER(flag_handler), __VA_ARGS__)
#define OPTION_BOOL(short_name, long_name, ...)                                                    \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_BOOL, HANDLER(bool_handler), __VA_ARGS__)
#define OPTION_STRING(short_name, long_name, ...)                                                  \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_STRING, HANDLER(string_handler), __VA_ARGS__)
#define OPTION_INT(short_name, long_name, ...)                                                     \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_INT, HANDLER(int_handler), __VA_ARGS__)
#define OPTION_FLOAT(short_name, long_name, ...)                                                   \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_FLOAT, HANDLER(float_handler), __VA_ARGS__)

#define OPTION_ARRAY_STRING(short_name, long_name, ...)                                            \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_STRING, HANDLER(array_string_handler),     \
                FREE_HANDLER(free_array_string_handler), __VA_ARGS__)
#define OPTION_ARRAY_INT(short_name, long_name, ...)                                               \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_INT, HANDLER(array_int_handler),           \
                __VA_ARGS__)
#define OPTION_ARRAY_FLOAT(short_name, long_name, ...)                                             \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_ARRAY_FLOAT, HANDLER(array_float_handler),       \
                __VA_ARGS__)

#define OPTION_MAP_STRING(short_name, long_name, ...)                                              \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_STRING, HANDLER(map_string_handler),         \
                FREE_HANDLER(free_map_string_handler), __VA_ARGS__)
#define OPTION_MAP_INT(short_name, long_name, ...)                                                 \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_INT, HANDLER(map_int_handler),               \
                FREE_HANDLER(free_map_int_handler), __VA_ARGS__)
#define OPTION_MAP_FLOAT(short_name, long_name, ...)                                               \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_FLOAT, HANDLER(map_float_handler),           \
                FREE_HANDLER(free_map_float_handler), __VA_ARGS__)
#define OPTION_MAP_BOOL(short_name, long_name, ...)                                                \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_MAP_BOOL, HANDLER(map_bool_handler),             \
                FREE_HANDLER(free_map_bool_handler), __VA_ARGS__)

/*
 * Common options
 */
#define HELP_OPTION()                                                                              \
    OPTION_FLAG('h', "help", HELP("Show help message"), HANDLER(help_handler), FLAGS(FLAG_EXIT))
#define VERSION_OPTION()                                                                           \
    OPTION_FLAG('V', "version", HELP("Show version information"), HANDLER(version_handler),        \
                FLAGS(FLAG_EXIT))

/*
 * Group macros
 */
#define GROUP_START(name, ...) GROUP_BASE(name, __VA_ARGS__)
#define GROUP_END()            GROUP_BASE(NULL)

/*
 * Positional argument macros
 */
#define POSITIONAL_STRING(name, ...)                                                               \
    POSITIONAL_BASE(name, VALUE_TYPE_STRING, HANDLER(string_handler), __VA_ARGS__)
#define POSITIONAL_INT(name, ...)                                                                  \
    POSITIONAL_BASE(name, VALUE_TYPE_INT, HANDLER(int_handler), __VA_ARGS__)
#define POSITIONAL_BOOL(name, ...)                                                                 \
    POSITIONAL_BASE(name, VALUE_TYPE_BOOL, HANDLER(bool_handler), __VA_ARGS__)
#define POSITIONAL_FLOAT(name, ...)                                                                \
    POSITIONAL_BASE(name, VALUE_TYPE_FLOAT, HANDLER(float_handler), __VA_ARGS__)

/*
 * Subcommand macro
 */
#define SUBCOMMAND(name, sub_options, ...) SUBCOMMAND_BASE(name, sub_options, __VA_ARGS__)

/*
 * Options array definition macro
 * @param name: Name of the options array
 * @param ...: Option definitions
 */
#define ARGUS_OPTIONS(name, ...)                                                                   \
    ARGUS_COMPILER_RULE_PUSH()                                                                     \
    argus_option_t name[] = {__VA_ARGS__ OPTION_END()};                                            \
    ARGUS_COMPILER_RULE_POP()

#endif /* ARGUS_OPTIONS_H */
