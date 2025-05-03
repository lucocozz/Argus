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
int flag_handler(argus_t *argus, argus_option_t *option, char *value);
int bool_handler(argus_t *argus, argus_option_t *option, char *value);
int string_handler(argus_t *argus, argus_option_t *option, char *value);
int int_handler(argus_t *argus, argus_option_t *option, char *value);
int float_handler(argus_t *argus, argus_option_t *option, char *value);
int help_handler(argus_t *argus, argus_option_t *option, char *value);
int version_handler(argus_t *argus, argus_option_t *option, char *value);
int default_free(argus_option_t *option);

int array_string_handler(argus_t *argus, argus_option_t *option, char *value);
int array_int_handler(argus_t *argus, argus_option_t *option, char *value);
int array_float_handler(argus_t *argus, argus_option_t *option, char *value);
int free_array_string_handler(argus_option_t *option);

int map_string_handler(argus_t *argus, argus_option_t *option, char *value);
int map_int_handler(argus_t *argus, argus_option_t *option, char *value);
int map_float_handler(argus_t *argus, argus_option_t *option, char *value);
int map_bool_handler(argus_t *argus, argus_option_t *option, char *value);
int free_map_string_handler(argus_option_t *option);
int free_map_int_handler(argus_option_t *option);
int free_map_float_handler(argus_option_t *option);
int free_map_bool_handler(argus_option_t *option);

int range_validator(argus_t *argus, argus_option_t *option, validator_data_t data);
int length_validator(argus_t *argus, argus_option_t *option, validator_data_t data);
int count_validator(argus_t *argus, argus_option_t *option, validator_data_t data);
int regex_validator(argus_t *argus, const char *value, validator_data_t data);

/*
 * Support macro for character to string conversion
 */
#define CHAR_TO_STRING(c) ((char[]){c, '\0'})

// clang-format off

/*
 * Optional option fields macros
 */
#define DEFINE_NAME(lname, sname) ((lname) ? (lname) : CHAR_TO_STRING(sname))
#define DEFAULT(val)            .value = (argus_value_t){ .raw = (uintptr_t)(val) },         \
                                .default_value = (argus_value_t){ .raw = (uintptr_t)(val) }, \
                                .is_set = true, \
                                .have_default = true
#define HANDLER(fn)             .handler = (argus_handler_t)(fn)
#define ACTION(fn)              .action = (argus_action_t)(fn)
#define FREE_HANDLER(fn)        .free_handler = (argus_free_handler_t)(fn)
#define HINT(_hint)             .hint = _hint
#define REQUIRES(...)           .requires = (const char*[]){__VA_ARGS__, NULL}
#define CONFLICTS(...)          .conflicts = (const char*[]){__VA_ARGS__, NULL}
#define GROUP_DESC(desc)        .help = desc
#define HELP(desc)              .help = desc
#define FLAGS(_flags)           .flags = _flags
#define ENV_VAR(name)           .env_name = name

/*
 * Validator macros
 */
#define VALIDATOR_AT(index, fn, _data) \
    .validator_count = ((index) + 1), \
    .validators[index].func = (argus_validator_t)(fn), \
    .validators[index].data = (validator_data_t){ .custom = (_data) }

#define VALIDATOR(fn, data)  VALIDATOR_AT(0, fn, data)
#define VALIDATOR2(fn, data) VALIDATOR_AT(1, fn, data)
#define VALIDATOR3(fn, data) VALIDATOR_AT(2, fn, data)
#define VALIDATOR4(fn, data) VALIDATOR_AT(3, fn, data)

#define RANGE(min, max) \
    .validators[0].func = (argus_validator_t)range_validator, \
    .validators[0].data = (validator_data_t){ .range = (range_t){ min, max } }, \
    .validator_count = 1
#define LENGTH(min, max) \
    .validators[0].func = (argus_validator_t)length_validator, \
    .validators[0].data = (validator_data_t){ .range = (range_t){ min, max } }, \
    .validator_count = 1
#define COUNT(min, max) \
    .validators[0].func = (argus_validator_t)count_validator, \
    .validators[0].data = (validator_data_t){ .range = (range_t){ min, max } }, \
    .validator_count = 1

#define PRE_VALIDATOR(fn, data) \
    .pre_validator = (argus_pre_validator_t)(fn), \
    .pre_validator_data = (validator_data_t){ .custom = (data) }

#define REGEX(re) \
    .pre_validator = (argus_pre_validator_t)regex_validator, \
    .pre_validator_data = (validator_data_t){ .regex = (re) }

/*
 * Choice macros for different types
 */
#define CHOICES_INT(...) \
    .choices = (argus_value_t){ .as_array_int = (long long[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((long long[]){ __VA_ARGS__ }) / sizeof(long long)

#define CHOICES_STRING(...) \
    .choices = (argus_value_t){ .as_array_string = (char*[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((char*[]){ __VA_ARGS__ }) / sizeof(char*)

#define CHOICES_FLOAT(...) \
    .choices = (argus_value_t){ .as_float_array = (double[]){ __VA_ARGS__ } }, \
    .choices_count = sizeof((double[]){ __VA_ARGS__ }) / sizeof(double)

/*
 * Base option definition macros
 */
#define OPTION_END() (argus_option_t){ \
    .type = TYPE_NONE, \
    .name = NULL, \
    .value_type = VALUE_TYPE_NONE \
}


#define OPTION_BASE(_short, _long, _value_type, ...)                                          \
    (argus_option_t) {                                                                        \
        .type = TYPE_OPTION, .name = DEFINE_NAME(_long, _short),                              \
        .sname = _short, .lname = _long, .value_type = _value_type,                           \
        .free_handler = default_free, ##__VA_ARGS__                                           \
    }

#define POSITIONAL_BASE(_name, _value_type, ...)                                               \
    (argus_option_t) {                                                                         \
        .type = TYPE_POSITIONAL, .name = _name, .value_type = _value_type,                     \
        .free_handler = default_free, .flags = FLAG_REQUIRED, ##__VA_ARGS__                    \
    }

#define GROUP_BASE(_name, ...)                                                                 \
    (argus_option_t) {                                                                         \
        .type = TYPE_GROUP, .name = _name, ##__VA_ARGS__                                       \
    }

#define SUBCOMMAND_BASE(_name, sub_opts, ...)                                                  \
    (argus_option_t) {                                                                         \
        .type = TYPE_SUBCOMMAND, .name = _name, .sub_options = sub_opts, ##__VA_ARGS__         \
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
#define HELP_OPTION(...)                                                                           \
    OPTION_FLAG('h', "help", HELP("Show help message"), HANDLER(help_handler), ##__VA_ARGS__)
#define VERSION_OPTION(...)                                                                        \
    OPTION_FLAG('V', "version", HELP("Show version information"), HANDLER(version_handler),        \
                ##__VA_ARGS__)

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
    PRAGMA_DISABLE_VARIADIC_MACROS()                                                               \
    PRAGMA_DISABLE_OVERRIDE()                                                                      \
    PRAGMA_DISABLE_PEDANTIC()                                                                      \
    argus_option_t name[] = {__VA_ARGS__, OPTION_END()};                                           \
    PRAGMA_RESTORE()                                                                               \
    PRAGMA_RESTORE()                                                                               \
    PRAGMA_RESTORE()

#endif /* ARGUS_OPTIONS_H */
