# Regular Expressions

argus offers powerful regular expression support via PCRE2 (Perl Compatible Regular Expressions) to validate user inputs with sophisticated pattern matching.

!!! abstract "Overview"
    This guide covers advanced usage of regular expressions in argus:
    
    - Using predefined patterns
    - Creating custom patterns
    - Pattern syntax and techniques
    - Best practices for regex validation
    
    For a complete list of predefined patterns, see the [Regular Expressions API Reference](../api/regex_patterns.md).

## Using Predefined Patterns

argus provides a wide range of predefined patterns in `argus/regex.h` that cover common validation scenarios.

=== "Basic Usage"
    ```c
    #include "argus/regex.h"
    
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(),
        
        // Email validation
        OPTION_STRING('e', "email", HELP("Email address"),
                    REGEX(ARGUS_RE_EMAIL)),
        
        // IP address validation
        OPTION_STRING('i', "ip", HELP("IP address"),
                    REGEX(ARGUS_RE_IPV4)),
        
        // URL validation
        OPTION_STRING('u', "url", HELP("URL"),
                    REGEX(ARGUS_RE_URL))
    )
    ```

=== "Multiple Patterns"
    ```c
    // Options with different types of validation
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(),
        
        // Date in ISO format (YYYY-MM-DD)
        OPTION_STRING('d', "date", HELP("Date (YYYY-MM-DD)"),
                    REGEX(ARGUS_RE_ISO_DATE),
                    HINT("YYYY-MM-DD")),
        
        // Password with strong validation
        OPTION_STRING('p', "password", HELP("Password (8+ chars, mixed case, numbers, symbols)"),
                    REGEX(ARGUS_RE_PASSWD_STRONG),
                    HINT("StrongP@ss1")),
        
        // Semantic version number
        OPTION_STRING('v', "version", HELP("Semantic version"),
                    REGEX(ARGUS_RE_SEMVER),
                    HINT("X.Y.Z"),
                    FLAGS(FLAG_REQUIRED))
    )
    ```

## Creating Custom Patterns

While predefined patterns cover many common needs, you can also create custom patterns for specific requirements.

### Using MAKE_REGEX

The `MAKE_REGEX` macro creates a new regex pattern with an error hint:

=== "Custom Pattern Definition"
    ```c
    // Define a custom pattern for product IDs
    #define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
    
    // Define a custom pattern for simple names
    #define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, numbers, underscore, dash")
    ```

=== "Using Custom Patterns"
    ```c
    ARGUS_OPTIONS(
        options,
        HELP_OPTION(),
        
        // Use custom patterns
        OPTION_STRING('p', "product", HELP("Product ID"),
                    REGEX(RE_PRODUCT_ID)),
        
        OPTION_STRING('n', "name", HELP("Username"),
                    REGEX(RE_SIMPLE_NAME))
    )
    ```

### Inline Patterns

You can also define patterns inline without creating a constant:

```c
OPTION_STRING('z', "zipcode", HELP("US ZIP code"),
            REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format: 12345 or 12345-6789")))
```

!!! note "MAKE_REGEX Structure"
    The `MAKE_REGEX` macro takes two parameters:
    
    1. The regular expression pattern
    2. An explanation text that will be displayed in case of validation failure

## Combining Patterns

For more complex validation requirements, you can create composite patterns:

```c
// Pattern for valid US phone numbers (multiple formats)
#define RE_US_PHONE_COMPLEX MAKE_REGEX(
    "^(\\+1[-\\s]?)?(\\([0-9]{3}\\)|[0-9]{3})[-\\s]?[0-9]{3}[-\\s]?[0-9]{4}$",
    "US phone format: 123-456-7890, (123) 456-7890, or +1 123 456 7890"
)
```

## Error Messages

A key advantage of argus' regex implementation is the ability to provide helpful error messages:

=== "Pattern with Detailed Error"
    ```c
    // Define a pattern with helpful error message
    #define RE_PASSWORD MAKE_REGEX(
        "^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$",
        "Password must contain at least 8 characters including uppercase, lowercase, digit and special character"
    )
    ```

=== "Error Output"
    ```
    my_program: Invalid value 'password123': Password must contain at least 8 characters including uppercase, lowercase, digit and special character
    ```

## Pattern Syntax

Since argus uses PCRE2, you have access to powerful pattern matching features:

### Common Pattern Elements

| Element | Description | Example |
|---------|-------------|---------|
| `^` | Start of string | `^abc` - String starts with "abc" |
| `$` | End of string | `xyz$` - String ends with "xyz" |
| `[]` | Character class | `[abc]` - Matches a, b, or c |
| `[^]` | Negated character class | `[^abc]` - Matches any character except a, b, or c |
| `\d` | Digit | `\d{3}` - Three digits |
| `\w` | Word character | `\w+` - One or more word characters |
| `\s` | Whitespace | `\s*` - Zero or more whitespace characters |
| `*` | Zero or more | `a*` - Zero or more "a" characters |
| `+` | One or more | `a+` - One or more "a" characters |
| `?` | Zero or one | `a?` - Zero or one "a" character |
| `{n}` | Exactly n times | `a{3}` - Exactly three "a" characters |
| `{n,m}` | Between n and m times | `a{2,4}` - Between 2 and 4 "a" characters |
| `(...)` | Capturing group | `(abc)+` - One or more occurrences of "abc" |
| `\1, \2, ...` | Backreferences | `(a)\\1` - "a" followed by another "a" |
| `a|b` | Alternation | `cat|dog` - Either "cat" or "dog" |

!!! warning "Escaping in C Strings"
    When writing regex patterns in C, remember to double-escape special characters:
    
    ```c
    // Double backslashes for regex special characters
    #define RE_DIGITS MAKE_REGEX("^\\d+$", "Numbers only")  // Note the double backslash
    ```

### Advanced Pattern Elements

PCRE2 also supports advanced features for complex validation needs:

| Element | Description | Example |
|---------|-------------|---------|
| `(?:...)` | Non-capturing group | `(?:abc)+` - One or more occurrences of "abc" without capturing |
| `(?=...)` | Positive lookahead | `a(?=b)` - "a" only if followed by "b" |
| `(?!...)` | Negative lookahead | `a(?!b)` - "a" only if not followed by "b" |
| `(?<=...)` | Positive lookbehind | `(?<=a)b` - "b" only if preceded by "a" |
| `(?<!...)` | Negative lookbehind | `(?<!a)b` - "b" only if not preceded by "a" |
| `(*UTF)` | Unicode mode | `(*UTF)\\p{L}+` - One or more Unicode letters |
| `\p{...}` | Unicode property | `\p{Lu}` - Uppercase letter |

## Performance Considerations

Regex validation can be powerful, but complex patterns can impact performance:

!!! tip "Performance Best Practices"
    1. **Keep patterns simple** when possible
    2. **Avoid excessive backtracking** (`.*` followed by specific matches)
    3. **Use anchors** (`^` and `$`) to prevent unnecessary scanning
    4. **Prefer non-capturing groups** (`(?:...)`) when you don't need captures

## Implementation Details

argus implements regex validation through the PCRE2 library:

```c
int regex_validator(argus_t *argus, const char *value, validator_data_t data)
{
    const char *pattern = data.regex.pattern;
    
    // Compile the regular expression
    pcre2_code *re = pcre2_compile(...);
    
    // Execute the regex against the input string
    int rc = pcre2_match(...);
    
    // Return validation result
    if (rc < 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                          "Invalid value '%s': %s", value, data.regex.hint);
    }
    return ARGUS_SUCCESS;
}
```

## Best Practices

When using regex validation with argus:

1. **Use predefined patterns** when possible for common validations
2. **Create descriptive error messages** that help the user understand what's required
3. **Keep patterns focused** on a single validation concern
4. **Document complex patterns** with comments explaining their purpose
5. **Test thoroughly** with valid and invalid inputs

## Complete Example

Here's a comprehensive example demonstrating multiple regex validation techniques:

```c
#include "argus.h"
#include "argus/regex.h"
#include <stdio.h>

/* Custom regex patterns */
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, numbers, underscore, dash")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),

    // Using predefined patterns
    GROUP_START("Network and Communication", GROUP_DESC("Network-related options")),
       OPTION_STRING('i', "ip", HELP("IPv4 address"),
                    REGEX(ARGUS_RE_IPV4)),

       OPTION_STRING('e', "email", HELP("Email address"),
                    REGEX(ARGUS_RE_EMAIL)),

        OPTION_STRING('u', "url", HELP("URL with any protocol"),
                    REGEX(ARGUS_RE_URL)),
    GROUP_END(),

    // Custom patterns defined above
    GROUP_START("Custom Formats", GROUP_DESC("Options with custom regex patterns")),
        OPTION_STRING('p', "product", HELP("Product ID (format: XX0000-XXXXXX)"),
                    REGEX(RE_PRODUCT_ID)),

        OPTION_STRING('n', "name", HELP("Username (letters, numbers, underscore, dash)"),
                    REGEX(RE_SIMPLE_NAME)),
    GROUP_END(),

    // Inline patterns
    GROUP_START("Inline Patterns", GROUP_DESC("Options with inline regex patterns")),
        OPTION_STRING('z', "zipcode", HELP("US Zip code"),
                    REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format: 12345 or 12345-6789"))),

        OPTION_STRING('t', "time", HELP("Time (format: HH:MM)"),
                    REGEX(MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9]$", "Format: HH:MM"))),
    GROUP_END()
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "regex_example", "1.0.0");
    argus.description = "Example of using regex validation with both predefined and custom patterns";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    printf("Validation successful! All provided values match the expected patterns.\n\n");

    // Display validated values
    printf("Network & Communication:\n");
    printf("  IP Address: %s\n", argus_is_set(argus, "ip") ?
           argus_get(argus, "ip").as_string : "(not provided)");
    printf("  Email: %s\n", argus_is_set(argus, "email") ?
           argus_get(argus, "email").as_string : "(not provided)");
    printf("  URL: %s\n", argus_is_set(argus, "url") ?
           argus_get(argus, "url").as_string : "(not provided)");

    printf("\nCustom Formats:\n");
    printf("  Product ID: %s\n", argus_is_set(argus, "product") ?
           argus_get(argus, "product").as_string : "(not provided)");
    printf("  Username: %s\n", argus_is_set(argus, "name") ?
           argus_get(argus, "name").as_string : "(not provided)");

    printf("\nInline Patterns:\n");
    printf("  Zip Code: %s\n", argus_is_set(argus, "zipcode") ?
           argus_get(argus, "zipcode").as_string : "(not provided)");
    printf("  Time: %s\n", argus_is_set(argus, "time") ?
           argus_get(argus, "time").as_string : "(not provided)");

    argus_free(&argus);
    return 0;
}
```

## Resources

For more information on regular expressions:

!!! tip "Learning Materials"
    - [Regular Expressions Info](https://www.regular-expressions.info/)
    - [PCRE2 Pattern Syntax](https://www.pcre.org/current/doc/html/pcre2pattern.html)
    - [RegExr: Online Testing Tool](https://regexr.com/)

## Related Documentation

- [Validation Guide](../guide/validation.md) - Basic validation concepts including regex
- [Regular Expressions API Reference](../api/regex_patterns.md) - Complete list of predefined patterns
- [Custom Validators](custom-validators.md) - Creating custom validators including pre-validators
