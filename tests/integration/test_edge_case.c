#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"
#include "argus/errors.h"
#include "argus/internal/parsing.h"

// Test options with special characters
ARGUS_OPTIONS(
    special_options,
    HELP_OPTION(),
    OPTION_STRING('s', "special-chars", HELP("Option with special characters")),
    POSITIONAL_STRING("arg", HELP("Argument with special characters"))
)

// Test options for map edge cases
ARGUS_OPTIONS(
    map_options,
    HELP_OPTION(),
    OPTION_MAP_STRING('m', "map", HELP("String map")),
    OPTION_MAP_INT('n', "numbers", HELP("Integer map")),
    OPTION_MAP_FLOAT('f', "floats", HELP("Float map")),
    OPTION_MAP_BOOL('b', "bools", HELP("Boolean map"))
)

// Test options for max/min value edge cases
ARGUS_OPTIONS(
    min_max_options,
    HELP_OPTION(),
    OPTION_INT('i', "int", HELP("Integer option")),
    OPTION_FLOAT('f', "float", HELP("Float option")),
    POSITIONAL_INT("pos-int", HELP("Positional integer"))
)

// Test valid positional arguments ordering (required first, then optional)
ARGUS_OPTIONS(
    valid_positional_options,
    HELP_OPTION(),
    POSITIONAL_STRING("required1", HELP("First required arg")),
    POSITIONAL_STRING("required2", HELP("Second required arg")),
    POSITIONAL_STRING("optional1", HELP("First optional arg"), FLAGS(FLAG_OPTIONAL)),
    POSITIONAL_STRING("optional2", HELP("Second optional arg"), FLAGS(FLAG_OPTIONAL))
)

// Helper function for output redirection
void setup_error_redirect(void)
{
    cr_redirect_stderr();
}

// Test handling special characters in option values
Test(edge_cases, special_chars_in_values)
{
    char *argv[] = {"test", "--special-chars=!@#$%^&*()_+", "~`<>,./?;:'\""};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(special_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing special characters should succeed");
    cr_assert_str_eq(argus_get(argus, "special-chars").as_string, "!@#$%^&*()_+",
                    "Option value with special chars should be parsed correctly");
    cr_assert_str_eq(argus_get(argus, "arg").as_string, "~`<>,./?;:'\"",
                    "Positional with special chars should be parsed correctly");
    
    argus_free(&argus);
}

// Test map option with empty keys and values
Test(edge_cases, map_empty_values)
{
    char *argv[] = {"test", "--map=empty=,=value,both="};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(map_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing maps with empty parts should succeed");
    cr_assert_str_eq(argus_map_get(argus, "map", "empty").as_string, "",
                    "Empty value should be handled correctly");
    // Testing empty key is tricky as map_get requires a key - it would need special handling
    
    argus_free(&argus);
}

// Test map option with negative keys and values
Test(edge_cases, map_negative_values)
{
    char *argv[] = {"test", "--numbers=neg=-42,minus-five=-5,positive=10"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(map_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing maps with negative values should succeed");
    cr_assert_eq(argus_map_get(argus, "numbers", "neg").as_int, -42,
                "Negative int value should be parsed correctly");
    cr_assert_eq(argus_map_get(argus, "numbers", "minus-five").as_int, -5,
                "Negative int value should be parsed correctly");
    cr_assert_eq(argus_map_get(argus, "numbers", "positive").as_int, 10,
                "Positive int value should be parsed correctly");
    
    argus_free(&argus);
}

// Test map option with floating point values
Test(edge_cases, map_float_values)
{
    char *argv[] = {"test", "--floats=pi=3.14159,neg=-2.718,zero=0.0,sci=1.23e-4"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(map_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing maps with float values should succeed");
    cr_assert_float_eq(argus_map_get(argus, "floats", "pi").as_float, 3.14159, 0.00001,
                      "Float value should be parsed correctly");
    cr_assert_float_eq(argus_map_get(argus, "floats", "neg").as_float, -2.718, 0.001,
                      "Negative float value should be parsed correctly");
    cr_assert_float_eq(argus_map_get(argus, "floats", "zero").as_float, 0.0, 0.001,
                      "Zero float value should be parsed correctly");
    // Scientific notation might not be supported, but it's good to test
    
    argus_free(&argus);
}

// Test map option with boolean values in various formats
Test(edge_cases, map_bool_values)
{
    char *argv[] = {"test", "--bools=t=true,f=false,y=yes,n=no,one=1,zero=0"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(map_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing maps with boolean values should succeed");
    cr_assert_eq(argus_map_get(argus, "bools", "t").as_bool, true,
                "true should be parsed as true");
    cr_assert_eq(argus_map_get(argus, "bools", "f").as_bool, false,
                "false should be parsed as false");
    cr_assert_eq(argus_map_get(argus, "bools", "y").as_bool, true,
                "yes should be parsed as true");
    cr_assert_eq(argus_map_get(argus, "bools", "n").as_bool, false,
                "no should be parsed as false");
    cr_assert_eq(argus_map_get(argus, "bools", "one").as_bool, true,
                "1 should be parsed as true");
    cr_assert_eq(argus_map_get(argus, "bools", "zero").as_bool, false,
                "0 should be parsed as false");
    
    argus_free(&argus);
}

// Test handling edge case integer values (INT_MAX, INT_MIN)
Test(edge_cases, integer_limits)
{
    char *argv[] = {"test", "--int=2147483647", "2147483647"};  // INT_MAX on many systems
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(min_max_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing INT_MAX should succeed");
    cr_assert_eq(argus_get(argus, "int").as_int, 2147483647,
                "INT_MAX should be parsed correctly");
    cr_assert_eq(argus_get(argus, "pos-int").as_int, 2147483647,
                "INT_MAX positional should be parsed correctly");
    
    argus_free(&argus);
}

// Test handling minimum integer values as positional arguments
Test(edge_cases, integer_min_without_separator)
{
    char *argv[] = {"test", "-2147483648"};  // INT_MIN on many systems
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(min_max_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing INT_MIN should succeed");
    cr_assert_eq(argus_get(argus, "pos-int").as_int, -2147483648,
                "INT_MIN positional should be parsed correctly");
    
    argus_free(&argus);
}

// Test valid positional argument configuration
Test(edge_cases, valid_positionals_full)
{
    // All arguments provided
    char *argv[] = {"test", "req1", "req2", "opt1", "opt2"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(valid_positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing all positionals should succeed");
    cr_assert_str_eq(argus_get(argus, "required1").as_string, "req1");
    cr_assert_str_eq(argus_get(argus, "required2").as_string, "req2");
    cr_assert_str_eq(argus_get(argus, "optional1").as_string, "opt1");
    cr_assert_str_eq(argus_get(argus, "optional2").as_string, "opt2");
    
    argus_free(&argus);
}

// Test with some optional positional arguments
Test(edge_cases, valid_positionals_some_optional)
{
    // Provide required args and only the first optional
    char *argv[] = {"test", "req1", "req2", "opt1"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(valid_positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing with some optionals should succeed");
    cr_assert_str_eq(argus_get(argus, "required1").as_string, "req1");
    cr_assert_str_eq(argus_get(argus, "required2").as_string, "req2");
    cr_assert_str_eq(argus_get(argus, "optional1").as_string, "opt1", "First optional should get value");
    cr_assert_eq(argus_is_set(argus, "optional2"), false, "Second optional should not be set");
    
    argus_free(&argus);
}

// Test with just required positional arguments
Test(edge_cases, valid_positionals_only_required)
{
    // Only provide required arguments
    char *argv[] = {"test", "req1", "req2"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(valid_positional_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing only required positionals should succeed");
    cr_assert_str_eq(argus_get(argus, "required1").as_string, "req1");
    cr_assert_str_eq(argus_get(argus, "required2").as_string, "req2");
    
    // Optional arguments shouldn't be set
    cr_assert_eq(argus_is_set(argus, "optional1"), false);
    cr_assert_eq(argus_is_set(argus, "optional2"), false);
    
    argus_free(&argus);
}
