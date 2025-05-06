/**
 * Validators example for argus
 * 
 * Demonstrates built-in validators and custom validators
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Custom validator for email addresses
int email_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    const char* email = option->value.as_string;
    if (!email) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email address cannot be NULL");
    }
    
    // Check for @ character
    const char* at = strchr(email, '@');
    if (!at) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email address must contain an '@' character");
    }
    
    // Check for domain
    const char* dot = strchr(at, '.');
    if (!dot) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email domain must contain a '.' character");
    }
    
    return ARGUS_SUCCESS;
}

// Custom validator for even numbers
int even_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    int number = option->value.as_int;
    if (number % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return ARGUS_SUCCESS;
}

// Custom validator for positive numbers
int positive_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    int number = option->value.as_int;
    if (number <= 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be a positive number");
    }
    
    return ARGUS_SUCCESS;
}

// Custom validator for alphanumeric strings
int alphanumeric_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    (void)data; // Unused parameter
    
    const char* str = option->value.as_string;
    if (!str) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "String cannot be NULL");
    }
    
    for (const char* p = str; *p; p++) {
        if (!isalnum(*p)) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                              "String must contain only alphanumeric characters");
        }
    }
    
    return ARGUS_SUCCESS;
}

// Custom validator for domain format in email
int domain_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    const char* domain = (const char*)data.custom;
    const char* email = option->value.as_string;
    
    if (!email) return ARGUS_SUCCESS; // Already checked in email_validator
    
    const char* at = strchr(email, '@');
    if (!at) return ARGUS_SUCCESS; // Already checked in email_validator
    
    // Check if email ends with the specified domain
    if (domain && !strstr(at + 1, domain)) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email must use the domain '%s'", domain);
    }
    
    return ARGUS_SUCCESS;
}

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Built-in range validator
    OPTION_INT('p', "port", HELP("Port number"), 
                DEFAULT(8080), RANGE(1, 65535)),
    
    // Built-in choices validator
    OPTION_STRING('l', "log-level", HELP("Log level"), 
                DEFAULT("info"), 
                CHOICES_STRING("debug", "info", "warning", "error")),
    
    // Built-in length validator combined with alphanumeric validator
    OPTION_STRING('u', "username", HELP("Username (3-16 alphanumeric chars)"),
                DEFAULT("user"), 
                LENGTH(3, 16),
                VALIDATOR2(alphanumeric_validator, NULL)),
                
    // Built-in count validator (for arrays and maps)
    OPTION_ARRAY_STRING('t', "tags", HELP("Tags (1-5 allowed)"),
                COUNT(1, 5)),
    
    // Multiple custom validators: email + specific domain
    OPTION_STRING('e', "email", HELP("Email address (company domain)"),
                VALIDATOR(email_validator, NULL),
                VALIDATOR2(domain_validator, "example.com")),
    
    // Multiple custom validators: even AND positive number
    OPTION_INT('n', "number", HELP("A positive even number"),
                VALIDATOR(even_validator, NULL),
                VALIDATOR2(positive_validator, NULL),
                DEFAULT(42))
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "validators_example", "1.0.0");
    argus.description = "Example of validators and multi-validators";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    int port = argus_get(argus, "port").as_int;
    const char* log_level = argus_get(argus, "log-level").as_string;
    const char* username = argus_get(argus, "username").as_string;
    int number = argus_get(argus, "number").as_int;
    const char* email = argus_is_set(argus, "email") ? 
                        argus_get(argus, "email").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (validator: range 1-65535)\n", port);
    printf("  Log level: %s (validator: choices)\n", log_level);
    printf("  Username: %s (validators: length 3-16 + alphanumeric)\n", username);
    printf("  Number: %d (validators: even + positive)\n", number);
    printf("  Email: %s (validators: email format + domain)\n", email);
    
    if (argus_is_set(argus, "tags")) {
        size_t count = argus_count(argus, "tags");
        argus_value_t *tags = argus_get(argus, "tags").as_array;
        printf("  Tags (%zu items, validator: count 1-5):\n", count);
        for (size_t i = 0; i < count; i++) {
            printf("    - %s\n", tags[i].as_string);
        }
    }
    
    argus_free(&argus);
    return 0;
}
