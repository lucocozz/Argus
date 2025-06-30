// tests/integration/test_validators.c
#include <criterion/criterion.h>
#include <ctype.h>
#include "argus.h"
#include "argus/regex.h"

// Test function for even numbers
int test_even_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    (void)data;
    argus_option_t *option = (argus_option_t *)option_ptr;
    int number = option->value.as_int;
    if (number % 2 != 0) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Value must be an even number");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    return ARGUS_SUCCESS;
}

// Test function for positive numbers
int test_positive_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    (void)data;
    argus_option_t *option = (argus_option_t *)option_ptr;
    int number = option->value.as_int;
    if (number <= 0) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Value must be a positive number");
        return ARGUS_ERROR_INVALID_VALUE;
    }
    return ARGUS_SUCCESS;
}

// Test function for alphanumeric validation
int test_alphanumeric_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    (void)data;
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *str = option->value.as_string;
    if (!str) return ARGUS_SUCCESS;
    
    for (const char* p = str; *p; p++) {
        if (!isalnum(*p)) {
            ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "String must contain only alphanumeric characters");
            return ARGUS_ERROR_INVALID_VALUE;
        }
    }
    return ARGUS_SUCCESS;
}

// Test function for divisibility
int test_divisible_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    int divisor = (int)data.custom;
    int value = option->value.as_int;
    
    if (value % divisor != 0) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Value must be divisible by %d", divisor);
        return ARGUS_ERROR_INVALID_VALUE;
    }
    
    return ARGUS_SUCCESS;
}

#define V_EVEN() \
    MAKE_VALIDATOR(test_even_validator, NULL, _V_DATA_CUSTOM_(NULL), ORDER_POST)
#define V_POSITIVE() \
    MAKE_VALIDATOR(test_positive_validator, NULL, _V_DATA_CUSTOM_(NULL), ORDER_POST)
#define V_ALPHANUMERIC() \
    MAKE_VALIDATOR(test_alphanumeric_validator, NULL, _V_DATA_CUSTOM_(NULL), ORDER_POST)
#define V_DIVISIBLE_BY(_divisor_) \
    MAKE_VALIDATOR(test_divisible_validator, NULL, _V_DATA_CUSTOM_(_divisor_), ORDER_POST)

// Define test options with validators
ARGUS_OPTIONS(
    test_options,
    HELP_OPTION(),
    OPTION_INT('p', "port", HELP("Port number"), 
                DEFAULT(8080), VALIDATOR(V_RANGE(1, 65535))),
    OPTION_STRING('l', "level", HELP("Log level"), 
                DEFAULT("info"), 
                VALIDATOR(V_CHOICE_STR("debug", "info", "warning", "error"))),
    OPTION_STRING('e', "email", HELP("Email address"), 
                VALIDATOR(V_REGEX(MAKE_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", 
                    "Enter email format")))),
    // Add LENGTH validator test option
    OPTION_STRING('u', "username", HELP("Username"), 
                VALIDATOR(V_LENGTH(3, 16))),
    // Add COUNT validator test option
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags"), 
                VALIDATOR(V_COUNT(2, 5))),
                
    // Multiple validators: test both even and positive
    OPTION_INT('n', "even-positive", HELP("Even positive number"),
                VALIDATOR(V_EVEN(), V_POSITIVE())),
                
    // Multiple validators: length and alphanumeric
    OPTION_STRING('a', "alphanum", HELP("Alphanumeric username"),
                VALIDATOR(V_LENGTH(3, 8), V_ALPHANUMERIC())),
                
    // Custom validator with data
    OPTION_INT('d', "divisible", HELP("Number divisible by 5"),
                VALIDATOR(V_DIVISIBLE_BY(5), V_RANGE(1, 100))),
)

// Test for range validation
Test(validators_integration, range_validation_success)
{
    // Valid port number
    char *argv[] = {"test", "-p", "8000"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid port should pass validation");
    cr_assert_eq(argus_get(&argus, "port").as_int, 8000);
    
    argus_free(&argus);
}

Test(validators_integration, range_validation_failure)
{
    // Invalid port number (out of range)
    char *argv[] = {"test", "-p", "90000"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Out of range port should fail validation");
    
    argus_free(&argus);
}

// Test for choices validation
Test(validators_integration, choices_validation_success)
{
    // Valid log level
    char *argv[] = {"test", "-l", "warning"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid log level should pass validation");
    cr_assert_str_eq(argus_get(&argus, "level").as_string, "warning");
    
    argus_free(&argus);
}

Test(validators_integration, choices_validation_failure)
{
    // Invalid log level
    char *argv[] = {"test", "-l", "critical"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Invalid log level should fail validation");
    
    argus_free(&argus);
}

// Test for regex validation
Test(validators_integration, regex_validation_success)
{
    // Valid email
    char *argv[] = {"test", "-e", "test@example.com"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid email should pass validation");
    cr_assert_str_eq(argus_get(&argus, "email").as_string, "test@example.com");
    
    argus_free(&argus);
}

Test(validators_integration, regex_validation_failure)
{
    // Invalid email
    char *argv[] = {"test", "-e", "invalid-email"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Invalid email should fail validation");
    
    argus_free(&argus);
}

// Test for length validation
Test(validators_integration, length_validation_success)
{
    // Valid username length
    char *argv[] = {"test", "-u", "johndoe"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid username length should pass validation");
    cr_assert_str_eq(argus_get(&argus, "username").as_string, "johndoe");
    
    argus_free(&argus);
}

Test(validators_integration, length_validation_failure_too_short)
{
    // Username too short
    char *argv[] = {"test", "-u", "jo"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Too short username should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, length_validation_failure_too_long)
{
    // Username too long
    char *argv[] = {"test", "-u", "johndoethisiswaytoolong"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Too long username should fail validation");
    
    argus_free(&argus);
}

// Test for count validation
Test(validators_integration, count_validation_success)
{
    // Valid number of tags
    char *argv[] = {"test", "-t", "tag1", "-t", "tag2", "-t", "tag3"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid number of tags should pass validation");
    cr_assert_eq(argus_count(&argus, "tags"), 3, "Should have 3 tags");
    
    argus_free(&argus);
}

Test(validators_integration, count_validation_failure_too_few)
{
    // Too few tags
    char *argv[] = {"test", "-t", "tag1"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);

    cr_assert_neq(status, ARGUS_SUCCESS, "Too few tags should fail validation");

    argus_free(&argus);
}

Test(validators_integration, count_validation_failure_too_many)
{
    // Too many tags
    char *argv[] = {"test", "-t", "tag1", "-t", "tag2", "-t", "tag3", 
                    "-t", "tag4", "-t", "tag5", "-t", "tag6"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Too many tags should fail validation");

    argus_free(&argus);
}

// Tests for multiple validators
Test(validators_integration, multiple_validators_all_pass)
{
    // Valid even positive number
    char *argv[] = {"test", "-n", "42"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid even positive number should pass all validators");
    cr_assert_eq(argus_get(&argus, "even-positive").as_int, 42);
    
    argus_free(&argus);
}

Test(validators_integration, multiple_validators_first_fails)
{
    // Invalid: not even (but still positive)
    char *argv[] = {"test", "-n", "43"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Non-even positive number should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, multiple_validators_second_fails)
{
    // Invalid: not positive (but still even)
    char *argv[] = {"test", "-n", "-2"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Negative even number should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, string_multiple_validators_all_pass)
{
    // Valid alphanumeric username with valid length
    char *argv[] = {"test", "-a", "user123"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid alphanumeric username should pass all validators");
    cr_assert_str_eq(argus_get(&argus, "alphanum").as_string, "user123");
    
    argus_free(&argus);
}

Test(validators_integration, string_multiple_validators_first_fails)
{
    // Invalid: too short
    char *argv[] = {"test", "-a", "ab"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Too short username should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, string_multiple_validators_second_fails)
{
    // Invalid: not alphanumeric but correct length
    char *argv[] = {"test", "-a", "user@12"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Non-alphanumeric username should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, string_multiple_validators_both_fail)
{
    // Invalid: too long and not alphanumeric
    char *argv[] = {"test", "-a", "user@12345678"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Username that violates both validators should fail");
    
    argus_free(&argus);
}

// Tests for custom validator with data
Test(validators_integration, custom_validator_with_data_success)
{
    // Valid number divisible by 5
    char *argv[] = {"test", "-d", "25"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Number divisible by 5 should pass validation");
    cr_assert_eq(argus_get(&argus, "divisible").as_int, 25);
    
    argus_free(&argus);
}

Test(validators_integration, custom_validator_with_data_failure)
{
    // Invalid: not divisible by 5
    char *argv[] = {"test", "-d", "23"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Number not divisible by 5 should fail validation");
    
    argus_free(&argus);
}

Test(validators_integration, custom_validator_with_range_both_pass)
{
    // Valid: divisible by 5 and in range
    char *argv[] = {"test", "-d", "50"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Valid number should pass both validators");
    cr_assert_eq(argus_get(&argus, "divisible").as_int, 50);
    
    argus_free(&argus);
}

Test(validators_integration, custom_validator_range_fails)
{
    // Invalid: divisible by 5 but out of range
    char *argv[] = {"test", "-d", "105"};
    int argc = sizeof(argv) / sizeof(char*);

    argus_t argus = argus_init(test_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Out of range number should fail validation");
    
    argus_free(&argus);
}
