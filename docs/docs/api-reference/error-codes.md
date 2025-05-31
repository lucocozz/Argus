# Error Codes

Complete reference for Argus error codes and their meanings.

## Success Codes

| Code | Value | Description |
|------|-------|-------------|
| `ARGUS_SUCCESS` | 0 | Operation completed successfully |
| `ARGUS_SOULD_EXIT` | 1 | Program should exit (help/version shown) |

## Structure Errors

Errors detected during option definition validation:

| Code | When Triggered | Fix |
|------|----------------|-----|
| `ARGUS_ERROR_DUPLICATE_OPTION` | Same short/long name used twice | Use unique option names |
| `ARGUS_ERROR_INVALID_HANDLER` | Option missing handler function | Add `HANDLER()` macro |
| `ARGUS_ERROR_INVALID_DEFAULT` | Default value invalid for type | Fix default value type |
| `ARGUS_ERROR_INVALID_GROUP` | Group definition malformed | Check group syntax |
| `ARGUS_ERROR_INVALID_DEPENDENCY` | Invalid requires/conflicts | Fix dependency names |
| `ARGUS_ERROR_INVALID_FLAG` | Invalid flag for option type | Use correct flags |
| `ARGUS_ERROR_INVALID_POSITION` | Required positional after optional | Reorder positionals |
| `ARGUS_ERROR_MALFORMED_OPTION` | Option definition incomplete | Add required fields |
| `ARGUS_ERROR_MISSING_HELP` | No help option defined | Add `HELP_OPTION()` |

### Examples

```c
// ❌ ARGUS_ERROR_DUPLICATE_OPTION
OPTION_STRING('o', "output", HELP("Output file")),
OPTION_INT('o', "count", HELP("Count"))  // Same short name

// ❌ ARGUS_ERROR_INVALID_HANDLER  
OPTION_STRING('f', "file", HELP("File"))  // Missing HANDLER()

// ❌ ARGUS_ERROR_INVALID_POSITION
POSITIONAL_STRING("input", HELP("Input"), FLAGS(FLAG_OPTIONAL)),
POSITIONAL_STRING("output", HELP("Output"))  // Required after optional
```

## Parsing Errors

Errors during command-line argument parsing:

| Code | When Triggered | Example |
|------|----------------|---------|
| `ARGUS_ERROR_INVALID_ARGUMENT` | Unknown option provided | `--unknown-option` |
| `ARGUS_ERROR_MISSING_VALUE` | Option requires value but none given | `--output` (no file) |
| `ARGUS_ERROR_MISSING_REQUIRED` | Required option not provided | Missing `FLAG_REQUIRED` option |
| `ARGUS_ERROR_CONFLICTING_OPTIONS` | Conflicting options used together | `--verbose --quiet` |
| `ARGUS_ERROR_INVALID_FORMAT` | Value format incorrect | `--port abc` |
| `ARGUS_ERROR_EXCLUSIVE_GROUP` | Multiple options from exclusive group | `--gzip --bzip2` |
| `ARGUS_ERROR_INVALID_CHOICE` | Value not in allowed choices | `--format pdf` |
| `ARGUS_ERROR_INVALID_RANGE` | Numeric value out of range | `--port 99999` |

### User Messages

```bash
$ ./app --unknown
app: Unknown option: '--unknown'

$ ./app --output
app: Missing value for option: '--output'

$ ./app --port abc
app: Invalid value 'abc': expected number

$ ./app --count 150
app: Value 150 is out of range [1, 100]
```

## Execution Errors

Errors during subcommand execution:

| Code | When Triggered | Fix |
|------|----------------|-----|
| `ARGUS_ERROR_NO_COMMAND` | `argus_exec()` with no command | Check `argus_has_command()` first |

## Internal Errors

System-level errors:

| Code | When Triggered | Cause |
|------|----------------|-------|
| `ARGUS_ERROR_MEMORY` | Memory allocation failed | Out of memory |
| `ARGUS_ERROR_INTERNAL` | Internal library error | Bug in Argus |
| `ARGUS_ERROR_UNSUPPORTED` | Feature not supported | Compile-time option disabled |
| `ARGUS_ERROR_INVALID_VALUE` | Generic value error | Various validation failures |
| `ARGUS_ERROR_STACK_OVERFLOW` | Too many errors/subcommands | Reduce nesting |

## Error Handling

### Automatic Handling

```c
// Parsing errors are automatically displayed
if (argus_parse(&argus, argc, argv) != ARGUS_SUCCESS) {
    return 1;  // Error already printed with suggestion
}
```

### Manual Error Reporting

```c
// In custom handlers/validators
ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, 
                  "Invalid endpoint format '%s'", value);

// Error context is automatic (option name, subcommand, etc.)
```

### Error Stack

```c
// View all accumulated errors
argus_print_error_stack(&argus);

// Clear error stack
argus_clear_errors(&argus);
```

## Validation Errors

Specific validation error patterns:

### Range Validation
```c
OPTION_INT('p', "port", VALIDATOR(V_RANGE(1, 65535)))
// Error: "Value 99999 is out of range [1, 65535]"
```

### Length Validation
```c
OPTION_STRING('u', "user", VALIDATOR(V_LENGTH(3, 20)))
// Error: "Value 2 is out of length [3, 20]"
```

### Count Validation
```c
OPTION_ARRAY_STRING('t', "tags", VALIDATOR(V_COUNT(1, 5)))
// Error: "Values count 6 is out of [1, 5]"
```

### Regex Validation
```c
OPTION_STRING('e', "email", VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))
// Error: "Invalid value 'bad-email': Enter email: user@example.com"
```

### Choice Validation
```c
OPTION_STRING('f', "format", CHOICES_STRING("json", "xml"))
// Error: "Cannot be set to 'pdf'. Choose from ["json", "xml"]"
```

## Error Prevention

### Good Practices

```c
// ✅ Clear option names
OPTION_STRING('c', "config", HELP("Configuration file"))

// ✅ Sensible defaults
OPTION_INT('p', "port", DEFAULT(8080), VALIDATOR(V_RANGE(1, 65535)))

// ✅ Clear help text
OPTION_STRING('o', "output", HELP("Output file path"))

// ✅ Proper validation
OPTION_STRING('e', "email", VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))
```

### Common Mistakes

```c
// ❌ Missing help
OPTION_STRING('f', "file")  // No HELP()

// ❌ Invalid default
OPTION_INT('p', "port", DEFAULT("8080"))  // String default for int

// ❌ Missing validation
OPTION_INT('p', "port")  // No range check
```

## Error Code Constants

```c
typedef enum argus_error_type_e {
    // Success
    ARGUS_SUCCESS = 0,
    ARGUS_SOULD_EXIT,
    
    // Structure errors (1-10)
    ARGUS_ERROR_DUPLICATE_OPTION,
    ARGUS_ERROR_INVALID_HANDLER,
    ARGUS_ERROR_INVALID_DEFAULT,
    ARGUS_ERROR_INVALID_GROUP,
    ARGUS_ERROR_INVALID_DEPENDENCY,
    ARGUS_ERROR_INVALID_FLAG,
    ARGUS_ERROR_INVALID_POSITION,
    ARGUS_ERROR_MALFORMED_OPTION,
    ARGUS_ERROR_MISSING_HELP,
    
    // Parsing errors (11-20)
    ARGUS_ERROR_INVALID_ARGUMENT,
    ARGUS_ERROR_MISSING_VALUE,
    ARGUS_ERROR_MISSING_REQUIRED,
    ARGUS_ERROR_CONFLICTING_OPTIONS,
    ARGUS_ERROR_INVALID_FORMAT,
    ARGUS_ERROR_EXCLUSIVE_GROUP,
    ARGUS_ERROR_INVALID_CHOICE,
    ARGUS_ERROR_INVALID_RANGE,
    
    // Execution errors (21-25)
    ARGUS_ERROR_NO_COMMAND,
    
    // Internal errors (26-30)
    ARGUS_ERROR_MEMORY,
    ARGUS_ERROR_INTERNAL,
    ARGUS_ERROR_UNSUPPORTED,
    ARGUS_ERROR_INVALID_VALUE,
    ARGUS_ERROR_STACK_OVERFLOW
} argus_error_type_t;
```
