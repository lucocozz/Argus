# Custom Validators

This guide explains how to create and use custom validators with argus to implement specialized validation logic for command-line options.

## Overview

Validation is essential for ensuring that command-line inputs meet your application's requirements. While argus provides built-in validators like `RANGE()` and `REGEX()`, custom validators allow you to implement application-specific validation logic.

In this guide, you'll learn about:

- The two types of validators: **validators** and **pre-validators**
- How to create and use custom validators
- Techniques for passing configuration data to validators
- Best practices for validator implementation

## Understanding Validator Types

Argus supports custom validation functions with explicit ordering control, allowing precise control over when validation occurs in the parsing process:

### Validators (ORDER_POST)

**Purpose**: Check the **processed** value after type conversion

**When to use**: When validating based on the final data type (int, float, string, etc.)

**Function signature**:
```c
int validator_function(argus_t *argus, void *option_ptr, validator_data_t data);
```

**Key points**:
- The `option_ptr` parameter must be cast to `argus_option_t *`
- Access the converted value via `option->value.as_int`, `option->value.as_string`, etc.
- Runs after the raw string has been converted to the target type

### Pre-validators (ORDER_PRE)

**Purpose**: Check the **raw string** before it's processed

**When to use**: When you need to:
- Validate string format before parsing attempts
- Perform complex string validation
- Prevent type conversion errors

**Function signature**:
```c
int pre_validator_function(argus_t *argus, void *value_ptr, validator_data_t data);
```

**Key points**:
- The `value_ptr` parameter should be cast to `const char *`
- Operates on the raw command-line string value
- Runs before any type conversion occurs

## Creating Basic Validators

Let's start with simple examples of both validator types.

### Example: Even Number Validator

This validator ensures that integer options have even values:

```c
int even_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    
    // Not using custom data in this example
    (void)data;
    
    if (option->value.as_int % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Value must be an even number, got %d", option->value.as_int);
    }
    return ARGUS_SUCCESS;
}
```

**Using the validator**:

```c
// Create a helper macro
#define V_EVEN() \
    MAKE_VALIDATOR(even_validator, _V_DATA_CUSTOM_(NULL), ORDER_POST)

OPTION_INT('n', "number", HELP("An even number"), 
          VALIDATOR(V_EVEN()))
```

### Example: String Length Pre-validator

This pre-validator checks if a string meets a minimum length requirement:

```c
int string_length_pre_validator(argus_t *argus, void *value_ptr, validator_data_t data)
{
    const char *value = (const char *)value_ptr;
    size_t min_length = (size_t)data.custom;
    
    if (strlen(value) < min_length) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "String must be at least %zu characters long", min_length);
    }
    return ARGUS_SUCCESS;
}
```

**Using the pre-validator**:

```c
// Create a helper macro
#define V_MIN_LENGTH(len) \
    MAKE_VALIDATOR(string_length_pre_validator, _V_DATA_CUSTOM_(len), ORDER_PRE)

OPTION_STRING('p', "password", HELP("Password"),
             VALIDATOR(V_MIN_LENGTH(8)))
```

## Passing Data to Validators

The `validator_data_t` parameter allows you to pass configuration data to your validators, making them more flexible and reusable. With the new API, data is passed using helper macros.

### Using the Custom Data Field

For simple data types, use `_V_DATA_CUSTOM_()` to pass values:

```c
int divisible_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    int divisor = (int)data.custom;  // Extract the divisor
    
    if (option->value.as_int % divisor != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    return ARGUS_SUCCESS;
}

// Helper macro for reusability
#define V_DIVISIBLE_BY(n) \
    MAKE_VALIDATOR(divisible_validator, _V_DATA_CUSTOM_(n), ORDER_POST)

// Usage
OPTION_INT('n', "number", HELP("Number divisible by 5"), 
          VALIDATOR(V_DIVISIBLE_BY(5)))
```

### Using Custom Data Structures

For complex validation rules, you can create a structure to hold multiple parameters:

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_odd;
} number_constraints_t;

int number_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Get constraints from validator data
    number_constraints_t *constraints = (number_constraints_t *)data.custom;
    
    // Range validation
    if (option->value.as_int < constraints->min_value || 
        option->value.as_int > constraints->max_value) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                          "Value must be between %d and %d", 
                          constraints->min_value, constraints->max_value);
    }
    
    // Even/odd validation
    if (!constraints->allow_odd && (option->value.as_int % 2 != 0)) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be an even number");
    }
    
    return ARGUS_SUCCESS;
}
```

**Using the validator with custom data**:

```c
// Define constraints
static number_constraints_t constraints = {
    .min_value = 10,
    .max_value = 100,
    .allow_odd = false
};

OPTION_INT('n', "number", HELP("A number with constraints"), 
          VALIDATOR(number_validator, &constraints))
```

### Using Inline Compound Literals

For simple cases, you can use C99 compound literals to pass data inline:

```c
OPTION_STRING('u', "username", HELP("Username"),
             VALIDATOR(V_MIN_LENGTH(3)))
```

This creates a reusable validator macro with the desired minimum length.

## Advanced Validation Techniques

### Context-Aware Validation

Sometimes validators need to check values in relation to other options:

```c
typedef struct {
    const char *related_option;
} option_relation_t;

int greater_than_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    option_relation_t *relation = (option_relation_t *)data.custom;
    argus_value_t other_value = argus_get(*argus, relation->related_option);
    
    if (option->value.as_int <= other_value.as_int) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be greater than '%s' (%d)",
                          relation->related_option, other_value.as_int);
    }
    
    return ARGUS_SUCCESS;
}
```

**Usage example**:
```c
static option_relation_t max_relation = { .related_option = "min" };

ARGUS_OPTIONS(
    options,
    OPTION_INT('n', "min", HELP("Minimum value")),
    OPTION_INT('x', "max", HELP("Maximum value"), 
               VALIDATOR(greater_than_validator, &max_relation))
)
```

### Creating Helper Macros

For frequently used validation patterns, create helper macros:

```c
// Helper macro for even number validation
#define V_EVEN() \
    MAKE_VALIDATOR(even_validator, _V_DATA_CUSTOM_(NULL), ORDER_POST)

// Helper macro for minimum string length
#define V_MIN_LENGTH(min) \
    MAKE_VALIDATOR(string_length_pre_validator, _V_DATA_CUSTOM_(min), ORDER_PRE)

// Helper macro for maximum string length
#define V_MAX_LENGTH(max) \
    MAKE_VALIDATOR(string_length_max_validator, _V_DATA_CUSTOM_(max), ORDER_PRE)

// Combined length check
#define V_STRING_LENGTH(min, max) \
    MAKE_VALIDATOR(string_length_range_validator, \
        _V_DATA_CUSTOM_(&((length_range_t){min, max})), ORDER_PRE)
```

**Usage example**:
```c
ARGUS_OPTIONS(
    options,
    OPTION_INT('n', "number", HELP("An even number"), EVEN_NUMBER()),
    OPTION_STRING('p', "password", HELP("Password"), MIN_LENGTH(8)),
    OPTION_STRING('u', "username", HELP("Username"), STRING_LENGTH(3, 20))
)
```

## Combining Multiple Validators

Argus allows you to apply multiple validators to a single option by using the numbered validator macros:

```c
OPTION_INT('p', "port", HELP("Port number"), 
          VALIDATOR(is_even_validator, NULL),      // First validator
          VALIDATOR2(range_validator, &port_range), // Second validator
          VALIDATOR3(port_validator, NULL))        // Third validator
```

Argus has a limit of 4 validators per option, but you can modify the constant `ARGUS_MAX_VALIDATORS` to increase this limit.

Note that built-in validators like `RANGE()`, `LENGTH()`, and `COUNT()` use the first validator slot. You can combine them with custom validators by using the second and subsequent slots:

```c
OPTION_INT('p', "port", HELP("Port number"),
          RANGE(1, 65535),                     // Uses first validator slot
          VALIDATOR2(is_even_validator, NULL)) // Uses second validator slot
```

## Error Reporting

Validators should use `ARGUS_REPORT_ERROR` to provide clear error messages:

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

**Common error codes**:

| Error Code | Description | Typical Use |
|------------|-------------|-------------|
| `ARGUS_ERROR_INVALID_VALUE` | Value doesn't meet requirements | General validation failures |
| `ARGUS_ERROR_INVALID_RANGE` | Value outside allowed range | Range validation |
| `ARGUS_ERROR_INVALID_FORMAT` | Value has incorrect format | Format validation |
| `ARGUS_ERROR_MEMORY` | Memory allocation failed | During validation processing |

## Best Practices

### 1. Single Responsibility

Each validator should focus on one validation concern:

```c
// Good: Two focused validators
int is_even_validator(argus_t *argus, argus_option_t *option, validator_data_t data);
int in_range_validator(argus_t *argus, argus_option_t *option, validator_data_t data);

// Use them together
OPTION_INT('n', "number", HELP("Number"), 
          VALIDATOR(is_even_validator, NULL),
          VALIDATOR2(in_range_validator, &range))
```

### 2. Descriptive Error Messages

Provide clear, actionable error messages:

```c
// Good: Clear and specific message
ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                  "Username must be 3-20 characters with only letters, numbers, and underscores");

// Bad: Vague message
ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Invalid input");
```

### 3. Parameter Safety

Always validate parameters and handle edge cases:

```c
int string_length_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Check if value is NULL before using it
    if (option->value.as_string == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "String cannot be NULL");
    }
    
    // Rest of the validation logic...
    return ARGUS_SUCCESS;
}
```

### 4. Memory Efficiency

Avoid unnecessary heap allocations in validators:

```c
int efficient_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    // Use stack-based buffers for temporary operations
    char buffer[256];
    
    // Process value without heap allocations
    
    return ARGUS_SUCCESS;
}
```

### 5. Reusable Components

Design validators to be reusable across options:

```c
// Generic validator for checking if a number is divisible by n
int divisible_by_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
{
    int divisor = *(int *)data.custom;
    
    if (option->value.as_int % divisor != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    
    return ARGUS_SUCCESS;
}

// Reuse with different configurations
OPTION_INT('n', "number", HELP("Number divisible by 2"), 
          VALIDATOR(divisible_by_validator, &((int){2})));

OPTION_INT('m', "multiple", HELP("Multiple of 5"), 
          VALIDATOR(divisible_by_validator, &((int){5})));
```

## Complete Example

Here's a complete example demonstrating various custom validator techniques:

```c
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

// Custom pre-validator for string case requirements
int case_pre_validator(argus_t *argus, const char *value, validator_data_t data)
{
    typedef enum { LOWERCASE, UPPERCASE, MIXED } case_requirement_t;
    case_requirement_t req = *(case_requirement_t *)data.custom;
    
    bool has_upper = false;
    bool has_lower = false;
    
    for (const char *p = value; *p; p++) {
        if (isupper(*p)) has_upper = true;
        if (islower(*p)) has_lower = true;
    }
    
    switch (req) {
        case LOWERCASE:
            if (has_upper) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must be lowercase only");
            }
            break;
            
        case UPPERCASE:
            if (has_lower) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must be uppercase only");
            }
            break;
            
        case MIXED:
            if (!has_upper || !has_lower) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                 "Value must contain both uppercase and lowercase letters");
            }
            break;
    }
    
    return ARGUS_SUCCESS;
}

// Helper macros for common validations
#define EVEN_NUMBER() VALIDATOR(even_validator, NULL)
#define EMAIL_VALIDATOR() VALIDATOR(email_validator, NULL)
#define LOWERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){LOWERCASE}))
#define UPPERCASE_ONLY() PRE_VALIDATOR(case_pre_validator, &((int){UPPERCASE}))
#define MIXED_CASE() PRE_VALIDATOR(case_pre_validator, &((int){MIXED}))

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
    
    // Custom email validator
    OPTION_STRING('e', "email", HELP("Email address"),
                EMAIL_VALIDATOR()),
    
    // Custom even number validator
    OPTION_INT('n', "number", HELP("An even number"),
                EVEN_NUMBER(),
                DEFAULT(42)),
    
    // String with case validation
    OPTION_STRING('u', "username", HELP("Username (lowercase)"),
                LOWERCASE_ONLY()),
                
    // String with multiple validators
    OPTION_STRING('p', "password", HELP("Password (mixed case)"),
                MIXED_CASE())
)

int main(int argc, char **argv) {
    argus_t argus = argus_init(options, "validators_example", "1.0.0");
    argus.description = "Example of custom validators";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Access parsed values
    int port = argus_get(argus, "port").as_int;
    const char* log_level = argus_get(argus, "log-level").as_string;
    int number = argus_get(argus, "number").as_int;
    
    const char* email = argus_is_set(argus, "email") ? 
                        argus_get(argus, "email").as_string : "not set";
    
    const char* username = argus_is_set(argus, "username") ?
                          argus_get(argus, "username").as_string : "not set";
    
    const char* password = argus_is_set(argus, "password") ?
                          argus_get(argus, "password").as_string : "not set";
    
    printf("Validated values:\n");
    printf("  Port: %d (range: 1-65535)\n", port);
    printf("  Log level: %s (choices: debug, info, warning, error)\n", log_level);
    printf("  Even number: %d (must be even)\n", number);
    printf("  Email: %s (must be valid email format)\n", email);
    printf("  Username: %s (must be lowercase)\n", username);
    printf("  Password: %s (must contain mixed case)\n", password);
    
    argus_free(&argus);
    return 0;
}
```

## Related Documentation

- [Validation Guide](../guide/validation.md) - Basic validation concepts
- [Regular Expressions Guide](regex.md) - Validation with regex patterns
