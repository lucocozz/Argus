/**
 * test_smart_hint.c - Unit tests for Smart Hint logic
 *
 * Tests the Smart Hint decision making logic to ensure correct behavior
 * for different validator scenarios.
 */

#include <criterion/criterion.h>
#include <criterion/new/assert.h>
#include <string.h>

#include "argus.h"
#include "argus/internal/help.h"
#include "argus/internal/display.h"

// Test helper - create a mock argus context with default configuration
static argus_t create_test_argus(void)
{
    argus_t argus = {0};
    argus.helper = (argus_helper_t){ .config = get_default_helper_config() };
    return argus;
}

// Test helper - create a mock option with validators
static argus_option_t create_test_option(validator_entry_t **validators, const char *hint)
{
    return (argus_option_t){
        .type = TYPE_OPTION,
        .name = "test",
        .sname = 't',
        .lname = "test",
        .value_type = VALUE_TYPE_STRING,
        .validators = validators,
        .hint = hint,
        .help = "Test option"
    };
}

// Test Smart Hint with HINT() override
Test(smart_hint, hint_override_priority)
{
    // HINT() should always take priority
    validator_entry_t *validators[] = {
        &(validator_entry_t){
            .func = range_validator,
            .data = (validator_data_t){.range = {1, 100}},
            .order = ORDER_POST,
            .formatter = format_range_validator
        },
        NULL
    };
    
    argus_option_t option = create_test_option(validators, "CUSTOM_HINT");
    
    argus_t argus = create_test_argus();
    char *result = get_smart_hint(&argus, &option);
    cr_assert_not_null(result, "Smart hint should not be null");
    cr_assert_str_eq(result, "CUSTOM_HINT", "HINT() should override validator formatter");
    
    free(result);
}

// Test Smart Hint with short validator format
Test(smart_hint, short_validator_in_hint)
{
    // Short validator format should go in hint
    validator_entry_t *validators[] = {
        &(validator_entry_t){
            .func = range_validator,
            .data = (validator_data_t){.range = {1, 100}},
            .order = ORDER_POST,
            .formatter = format_range_validator
        },
        NULL
    };
    
    argus_option_t option = create_test_option(validators, NULL);
    
    argus_t argus = create_test_argus();
    char *result = get_smart_hint(&argus, &option);
    cr_assert_not_null(result, "Smart hint should not be null");
    cr_assert_str_eq(result, "1-100", "Short validator format should be in hint");
    
    free(result);
}

// Test Smart Hint with long validator format (mock)
Test(smart_hint, long_validator_fallback)
{
    // For this test, we'll mock a long format by testing with choices
    validator_entry_t *validators[] = {
        &(validator_entry_t){
            .func = choices_string_validator,
            .data = (validator_data_t){.choices = {
                .as_strings = (char*[]){"very-long-option-name", "another-very-long-option", "third-extremely-long-choice", NULL},
                .count = 3,
                .type = VALUE_TYPE_STRING
            }},
            .order = ORDER_POST,
            .formatter = format_choices_validator
        },
        NULL
    };
    
    argus_option_t option = create_test_option(validators, NULL);
    
    argus_t argus = {0};  // Mock argus context
    char *result = get_smart_hint(&argus, &option);
    cr_assert_not_null(result, "Smart hint should not be null");
    
    // Should fallback to base type since choices are too long
    char *choices_formatted = format_choices_validator(validators[0]->data);
    if (strlen(choices_formatted) > 20) {
        cr_assert_str_eq(result, "STR", "Long validator should fallback to base type");
    }
    
    free(choices_formatted);
    free(result);
}

// Test Smart Hint with no validators
Test(smart_hint, no_validators_fallback)
{
    argus_option_t option = create_test_option(NULL, NULL);
    
    argus_t argus = {0};  // Mock argus context
    char *result = get_smart_hint(&argus, &option);
    cr_assert_not_null(result, "Smart hint should not be null");
    cr_assert_str_eq(result, "STR", "No validators should fallback to base type");
    
    free(result);
}

// Test Smart Hint with multiple validators
Test(smart_hint, multiple_validators_fallback)
{
    validator_entry_t *validators[] = {
        &(validator_entry_t){
            .func = length_validator,
            .data = (validator_data_t){.range = {8, 64}},
            .order = ORDER_POST,
            .formatter = format_length_validator
        },
        &(validator_entry_t){
            .func = regex_validator,
            .data = (validator_data_t){.regex = {"^[a-zA-Z0-9]+$", "alphanumeric"}},
            .order = ORDER_PRE,
            .formatter = format_regex_validator
        },
        NULL
    };
    
    argus_option_t option = create_test_option(validators, NULL);
    
    argus_t argus = {0};  // Mock argus context
    char *result = get_smart_hint(&argus, &option);
    cr_assert_not_null(result, "Smart hint should not be null");
    cr_assert_str_eq(result, "STR", "Multiple validators should fallback to base type");
    
    free(result);
}

// Test is_short_hint function
Test(smart_hint, short_hint_detection)
{
    argus_t argus = create_test_argus();
    
    cr_assert(is_short_hint(&argus, "1-100"), "Range should be detected as short");
    cr_assert(is_short_hint(&argus, "debug|info|warn"), "Short choices should be detected as short");
    cr_assert(is_short_hint(&argus, "8-64"), "Length should be detected as short");
    
    cr_assert_not(is_short_hint(&argus, "very long hint with spaces"), "Long hint with spaces should not be short");
    cr_assert_not(is_short_hint(&argus, "very-very-very-long-hint-without-spaces-but-too-long"), "Long hint without spaces should not be short");
    cr_assert_not(is_short_hint(&argus, "hint with spaces"), "Hint with spaces should not be short");
    cr_assert_not(is_short_hint(&argus, NULL), "NULL should not be short");
}

// Test has_single_validator function
Test(smart_hint, single_validator_detection)
{
    validator_entry_t *single[] = {
        &(validator_entry_t){
            .func = range_validator,
            .data = (validator_data_t){.range = {1, 100}},
            .order = ORDER_POST,
            .formatter = format_range_validator
        },
        NULL
    };
    
    validator_entry_t *multiple[] = {
        &(validator_entry_t){
            .func = range_validator,
            .data = (validator_data_t){.range = {1, 100}},
            .order = ORDER_POST,
            .formatter = format_range_validator
        },
        &(validator_entry_t){
            .func = length_validator,
            .data = (validator_data_t){.range = {8, 64}},
            .order = ORDER_POST,
            .formatter = format_length_validator
        },
        NULL
    };
    
    argus_option_t option_single = create_test_option(single, NULL);
    argus_option_t option_multiple = create_test_option(multiple, NULL);
    argus_option_t option_none = create_test_option(NULL, NULL);
    
    cr_assert(has_single_validator(&option_single), "Should detect single validator");
    cr_assert_not(has_single_validator(&option_multiple), "Should not detect multiple validators as single");
    cr_assert_not(has_single_validator(&option_none), "Should not detect no validators as single");
}
