# Validation

argus offers robust validation capabilities to ensure that command-line inputs meet your application's requirements.

!!! abstract "Overview"
    Validation in argus is organized into several categories:
    
    - **Built-in Validators** - Simple validation with `RANGE` and `CHOICES`
    - **Regular Expression Validation** - String pattern validation with PCRE2
    - **Custom Validators** - Create your own validation logic
    
    This guide covers the basics of each approach. For advanced usage, see the specialized guides linked throughout.

## Built-in Validators

argus provides several built-in validators to simplify common validation scenarios. These can be applied directly to option definitions.

### Range Validation

The `RANGE` validator ensures numeric values fall within a specified range:

```c
OPTION_INT('p', "port", HELP("Port number"),
           RANGE(1, 65535),  // Must be between 1 and 65535
           DEFAULT(8080))
```

### Length Validation

The `LENGTH` validator ensures string values have an appropriate length:

```c
OPTION_STRING('u', "username", HELP("Username"),
              LENGTH(3, 20),  // Must be between 3 and 20 characters
              DEFAULT("user"))
```

### Count Validation

The `COUNT` validator ensures collections (arrays and maps) have an appropriate number of elements:

```c
OPTION_ARRAY_STRING('t', "tags", HELP("Tags for the resource"),
                   COUNT(1, 5),  // Must have between 1 and 5 tags
                   FLAGS(FLAG_UNIQUE))
```

## Choices Validation

The `CHOICES` validator ensures the value is one of a specific set:

=== "String Choices"
    ```c
    OPTION_STRING('l', "level", HELP("Log level"),
                  CHOICES_STRING("debug", "info", "warning", "error"),
                  DEFAULT("info"))
    ```

=== "Integer Choices"
    ```c
    OPTION_INT('m', "mode", HELP("Operating mode"),
               CHOICES_INT(1, 2, 3),
               DEFAULT(1))
    ```

=== "Float Choices"
    ```c
    OPTION_FLOAT('s', "scale", HELP("Scale factor"),
                CHOICES_FLOAT(0.5, 1.0, 2.0),
                DEFAULT(1.0))
    ```

## Regular Expression Validation

argus uses PCRE2 for powerful regular expression validation:

=== "Basic Usage"
    ```c
    OPTION_STRING('e', "email", HELP("Email address"),
                  REGEX(ARGUS_RE_EMAIL))  // Must be a valid email
    ```

=== "Custom Pattern"
    ```c
    OPTION_STRING('i', "id", HELP("Product ID"),
                  REGEX(MAKE_REGEX("^[A-Z]{2}\\d{4}$", "Format: XX0000")))
    ```

!!! tip "Predefined Patterns"
    argus includes many predefined patterns in `argus/regex.h`:
    
    | Constant | Validates | Example |
    |----------|-----------|---------|
    | `ARGUS_RE_EMAIL` | Email addresses | user@example.com |
    | `ARGUS_RE_IPV4` | IPv4 addresses | 192.168.1.1 |
    | `ARGUS_RE_URL` | URLs | https://example.com |
    | `ARGUS_RE_ISO_DATE` | ISO format dates | 2023-01-31 |
    
    For a complete list, see the [Regular Expressions API reference](../api/regex.md).
    
    For advanced regex usage, see the [Regular Expressions guide](../advanced/regex.md).

## Custom Validators

For more complex validation logic, you can create your own validators:

=== "Simple Validator"
    ```c
    int even_validator(argus_t *argus, argus_option_t *option, validator_data_t data)
    {
        if (option->value.as_int % 2 != 0) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                             "Value must be an even number");
        }
        return ARGUS_SUCCESS;
    }
    
    // Usage
    OPTION_INT('n', "number", HELP("An even number"), 
              VALIDATOR(even_validator, NULL))
    ```

=== "Basic Pre-Validator"
    ```c
    int length_pre_validator(argus_t *argus, const char *value, validator_data_t data)
    {
        size_t min_length = *(size_t *)data.custom;
        
        if (strlen(value) < min_length) {
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                              "String must be at least %zu characters long", min_length);
        }
        return ARGUS_SUCCESS;
    }
    
    // Usage
    size_t min_length = 8;
    OPTION_STRING('p', "password", HELP("Password"),
                 PRE_VALIDATOR(length_pre_validator, &min_length))
    ```

!!! info "Validator Types"
    argus supports two types of custom validators:
    
    1. **Validators** - Validate the **processed** value after conversion to its final type
    2. **Pre-Validators** - Validate the **raw string** before any processing
    
    For a detailed exploration of custom validators, including examples and best practices, 
    see the [Custom Validators guide](../advanced/custom-validators.md).

## Combining Validators

You can apply multiple validators to a single option for more comprehensive validation:

```c
OPTION_INT('p', "port", HELP("Port number"), 
          RANGE(1, 65535),                   // Range validator
          VALIDATOR(even_validator, NULL),   // Custom validator
          DEFAULT(8080))
```

## Error Reporting

Validators should use `ARGUS_REPORT_ERROR` to provide clear error messages:

```c
ARGUS_REPORT_ERROR(argus, error_code, format_string, ...);
```

!!! example "Error Message Example"
    ```
    my_program: Value '70000' is outside the range [1, 65535] for option 'port'
    ```

Common error codes include:
- `ARGUS_ERROR_INVALID_VALUE`: Value doesn't meet requirements
- `ARGUS_ERROR_INVALID_RANGE`: Value outside allowed range
- `ARGUS_ERROR_INVALID_FORMAT`: Value has incorrect format

## Complete Examples

For complete working examples of validation techniques:

!!! tip "Example Files"
    - `examples/validators.c` - Demonstrates all validation techniques
    - `examples/regex.c` - Focused on regular expression validation

## Additional Resources

For more in-depth coverage of validation topics, refer to these advanced guides:

- [Custom Validators](../advanced/custom-validators.md) - Creating custom validators with specialized logic
- [Regular Expressions](../advanced/regex.md) - Detailed guide to regex pattern validation
- [Predefined Regex Patterns](../api/regex_patterns.md) - List of predefined regex patterns in argus
