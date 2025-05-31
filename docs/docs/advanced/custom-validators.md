# Custom Validators

Create specialized validation logic beyond built-in validators for application-specific requirements.

## Validator Types

Argus supports two validator execution timings:

| Type | When | Function Signature | Use Case |
|------|------|-------------------|----------|
| **Pre-validator** (`ORDER_PRE`) | Before type conversion | `int func(argus_t *argus, void *value_ptr, validator_data_t data)` | Raw string validation |
| **Post-validator** (`ORDER_POST`) | After type conversion | `int func(argus_t *argus, void *option_ptr, validator_data_t data)` | Typed value validation |

## Basic Custom Validators

import Tabs from '@theme/Tabs';
import TabItem from '@theme/TabItem';

<Tabs>
<TabItem value="post-validator" label="Post-Validator" default>

Validate converted values:

```c
// Even number validator (post-conversion)
int even_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    (void)data; // Unused
    
    if (option->value.as_int % 2 != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Value must be even, got %d", option->value.as_int);
    }
    return ARGUS_SUCCESS;
}

// Helper macro
#define V_EVEN() \
    MAKE_VALIDATOR(even_validator, _V_DATA_CUSTOM_(NULL), ORDER_POST)

// Usage
OPTION_INT('n', "number", HELP("Even number"), VALIDATOR(V_EVEN()))
```

</TabItem>
<TabItem value="pre-validator" label="Pre-Validator">

Validate raw string format:

```c
// Email format pre-validator (before parsing)
int email_format_validator(argus_t *argus, void *value_ptr, validator_data_t data)
{
    const char *email = (const char *)value_ptr;
    (void)data; // Unused
    
    // Basic format check
    const char *at = strchr(email, '@');
    if (!at) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                         "Email must contain '@' symbol");
    }
    
    const char *dot = strchr(at, '.');
    if (!dot) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                         "Email domain must contain '.' symbol");
    }
    
    return ARGUS_SUCCESS;
}

#define V_EMAIL_FORMAT() \
    MAKE_VALIDATOR(email_format_validator, _V_DATA_CUSTOM_(NULL), ORDER_PRE)

// Usage
OPTION_STRING('e', "email", HELP("Email address"), VALIDATOR(V_EMAIL_FORMAT()))
```

</TabItem>
</Tabs>

## Passing Data to Validators

Use `validator_data_t` to pass configuration:

<Tabs>
<TabItem value="simple-data" label="Simple Data" default>

```c
// Divisible-by validator with parameter
int divisible_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    int divisor = (int)data.custom;
    
    if (option->value.as_int % divisor != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Value must be divisible by %d", divisor);
    }
    return ARGUS_SUCCESS;
}

#define V_DIVISIBLE_BY(n) \
    MAKE_VALIDATOR(divisible_validator, _V_DATA_CUSTOM_(n), ORDER_POST)

// Usage
OPTION_INT('p', "port", HELP("Port (multiple of 100)"), 
          VALIDATOR(V_DIVISIBLE_BY(100)))
```

</TabItem>
<TabItem value="struct-data" label="Complex Data">

```c
typedef struct {
    int min_value;
    int max_value;
    bool allow_zero;
} range_config_t;

int advanced_range_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    range_config_t *config = (range_config_t *)data.custom;
    int value = option->value.as_int;
    
    if (!config->allow_zero && value == 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Zero not allowed");
    }
    
    if (value < config->min_value || value > config->max_value) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_RANGE,
                         "Value %d out of range [%d, %d]", 
                         value, config->min_value, config->max_value);
    }
    
    return ARGUS_SUCCESS;
}

#define V_ADVANCED_RANGE(min, max, allow_zero) \
    MAKE_VALIDATOR(advanced_range_validator, \
        _V_DATA_CUSTOM_(&((range_config_t){min, max, allow_zero})), \
        ORDER_POST)

OPTION_INT('p', "port", HELP("Privileged port"), 
          VALIDATOR(V_ADVANCED_RANGE(1024, 65535, false)))
```

</TabItem>
</Tabs>

## Context-Aware Validation

Access other options for cross-validation:

```c
// Validate that max > min
int max_greater_than_min_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *min_option = (const char *)data.custom;
    
    int max_value = option->value.as_int;
    int min_value = argus_get(*argus, min_option).as_int;
    
    if (max_value <= min_value) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Max value %d must be greater than min value %d",
                         max_value, min_value);
    }
    
    return ARGUS_SUCCESS;
}

#define V_GREATER_THAN(option_name) \
    MAKE_VALIDATOR(max_greater_than_min_validator, _V_DATA_CUSTOM_(option_name), ORDER_POST)

ARGUS_OPTIONS(
    options,
    OPTION_INT('m', "min", HELP("Minimum value")),
    OPTION_INT('M', "max", HELP("Maximum value"), 
              VALIDATOR(V_GREATER_THAN("min"))),
)
```

## Advanced Examples

<Tabs>
<TabItem value="domain-validator" label="Domain-Specific" default>

```c
// Company email domain validator
int company_email_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *required_domain = (const char *)data.custom;
    const char *email = option->value.as_string;
    
    const char *at = strchr(email, '@');
    if (!at) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Invalid email format");
    }
    
    if (strcmp(at + 1, required_domain) != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "Email must use domain '%s'", required_domain);
    }
    
    return ARGUS_SUCCESS;
}

#define V_COMPANY_EMAIL(domain) \
    MAKE_VALIDATOR(company_email_validator, _V_DATA_CUSTOM_(domain), ORDER_POST)

// Usage
OPTION_STRING('e', "email", HELP("Company email"), 
             VALIDATOR(V_COMPANY_EMAIL("company.com")))
```

</TabItem>
<TabItem value="case-validator" label="String Case">

```c
typedef enum { LOWERCASE, UPPERCASE, MIXED } case_requirement_t;

int case_validator(argus_t *argus, void *value_ptr, validator_data_t data)
{
    const char *value = (const char *)value_ptr;
    case_requirement_t req = *(case_requirement_t *)data.custom;
    
    bool has_upper = false, has_lower = false;
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
                                 "Value must contain both upper and lowercase");
            }
            break;
    }
    
    return ARGUS_SUCCESS;
}

#define V_LOWERCASE() \
    MAKE_VALIDATOR(case_validator, _V_DATA_CUSTOM_(&(case_requirement_t){LOWERCASE}), ORDER_PRE)
#define V_UPPERCASE() \
    MAKE_VALIDATOR(case_validator, _V_DATA_CUSTOM_(&(case_requirement_t){UPPERCASE}), ORDER_PRE)
#define V_MIXED_CASE() \
    MAKE_VALIDATOR(case_validator, _V_DATA_CUSTOM_(&(case_requirement_t){MIXED}), ORDER_PRE)
```

</TabItem>
<TabItem value="file-validator" label="File System">

```c
// File existence validator
int file_exists_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *filepath = option->value.as_string;
    bool must_exist = (bool)data.custom;
    
    FILE *file = fopen(filepath, "r");
    bool exists = (file != NULL);
    if (file) fclose(file);
    
    if (must_exist && !exists) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "File '%s' does not exist", filepath);
    }
    
    if (!must_exist && exists) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                         "File '%s' already exists", filepath);
    }
    
    return ARGUS_SUCCESS;
}

#define V_FILE_EXISTS() \
    MAKE_VALIDATOR(file_exists_validator, _V_DATA_CUSTOM_(true), ORDER_POST)
#define V_FILE_NOT_EXISTS() \
    MAKE_VALIDATOR(file_exists_validator, _V_DATA_CUSTOM_(false), ORDER_POST)

// Usage
OPTION_STRING('i', "input", HELP("Input file"), VALIDATOR(V_FILE_EXISTS()))
OPTION_STRING('o', "output", HELP("Output file"), VALIDATOR(V_FILE_NOT_EXISTS()))
```

</TabItem>
</Tabs>

## Collection Validation

Validate array and map contents:

```c
// Array element validator
int array_all_positive_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    (void)data; // Unused
    
    for (size_t i = 0; i < option->value_count; i++) {
        int value = option->value.as_array[i].as_int;
        if (value <= 0) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                             "All values must be positive, got %d at index %zu", 
                             value, i);
        }
    }
    
    return ARGUS_SUCCESS;
}

#define V_ALL_POSITIVE() \
    MAKE_VALIDATOR(array_all_positive_validator, _V_DATA_CUSTOM_(NULL), ORDER_POST)

// Usage
OPTION_ARRAY_INT('p', "ports", HELP("Port numbers"), 
                VALIDATOR(V_COUNT(1, 10), V_ALL_POSITIVE()))
```

## Combining Validators

Multiple validators execute in order:

```c
ARGUS_OPTIONS(
    options,
    // Multiple validators: pre-validation, then post-validation
    OPTION_STRING('p', "password", HELP("Secure password"),
                 VALIDATOR(V_LENGTH(8, 128),           // Built-in
                          V_MIXED_CASE(),             // Custom pre-validator
                          V_NO_COMMON_PASSWORDS())),  // Custom post-validator
                          
    // Range + custom validation
    OPTION_INT('p', "port", HELP("Even port number"),
              VALIDATOR(V_RANGE(1, 65535), V_EVEN())),
)
```

**Execution order:**
1. Pre-validators (`ORDER_PRE`)
2. Type conversion
3. Post-validators (`ORDER_POST`)
4. Built-in validation (choices, etc.)

## Best Practices

```c
// ✅ Good validator design
int descriptive_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    // Single responsibility
    // Clear error messages
    // Proper error codes
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                     "Username must start with a letter and contain only alphanumeric characters");
}

// ✅ Reusable with parameters
#define V_MIN_WORDS(count) \
    MAKE_VALIDATOR(min_words_validator, _V_DATA_CUSTOM_(count), ORDER_POST)

// ❌ Avoid these patterns
int bad_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    // Too many responsibilities
    // Vague error message
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Invalid input");
}
```

## What's Next?

- **[Custom Handlers](custom-handlers)** - Parse complex input formats
- **[Validation](../features/validation)** - Built-in validation options
- **[Collections](../features/collections)** - Validate arrays and maps

:::tip Validator Design
Focus on single responsibility and clear error messages. Users should understand exactly what's wrong and how to fix it.
:::
