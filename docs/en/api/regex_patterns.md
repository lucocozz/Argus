# Regular Expressions API Reference

argus includes a rich set of predefined regular expression patterns in the `argus/regex.h` header. These patterns can be used with the `REGEX()` validator to validate option values against common formats.

!!! tip "Usage"
    To use these patterns, include the regex header and apply the pattern with the `REGEX()` validator:
    
    ```c
    #include "argus/regex.h"
    
    OPTION_STRING('e', "email", HELP("Email address"), 
                 REGEX(ARGUS_RE_EMAIL))
    ```

## Network and Communication

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_IPV4` | IPv4 address | `xxx.xxx.xxx.xxx` | 192.168.1.1 |
| `ARGUS_RE_IP4CIDR` | IPv4 with CIDR suffix | `xxx.xxx.xxx.xxx/xx` | 192.168.1.0/24 |
| `ARGUS_RE_IPV6` | IPv6 address | `xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx` | 2001:0db8:85a3:0000:0000:8a2e:0370:7334 |
| `ARGUS_RE_MAC` | MAC address | `xx:xx:xx:xx:xx:xx` or `xx-xx-xx-xx-xx-xx` | 01:23:45:67:89:AB |
| `ARGUS_RE_DOMAIN` | Fully Qualified Domain Name | `example.com` | mydomain.example.com |
| `ARGUS_RE_URL` | URL with any protocol | `protocol://domain/path` | https://example.com/path |
| `ARGUS_RE_HTTP` | HTTP/HTTPS URL | `http(s)://domain/path` | https://example.com |
| `ARGUS_RE_FILE_URL` | File URL | `file://path` | file:///path/to/file |
| `ARGUS_RE_PORT` | Port number (1-65535) | `1-65535` | 8080 |

## Email

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_EMAIL` | Basic email validation | `user@domain.tld` | user@example.com |
| `ARGUS_RE_EMAIL_STRICT` | Strict email validation (RFC compliant) | `user@domain.tld` | user.name+tag@example.co.uk |

## Date and Time

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_ISO_DATE` | ISO 8601 date | `YYYY-MM-DD` | 2023-01-31 |
| `ARGUS_RE_ISOTIME` | ISO 8601 datetime | `YYYY-MM-DDThh:mm:ss` | 2023-01-31T14:30:00 |
| `ARGUS_RE_US_DATE` | US date format | `MM/DD/YYYY` | 01/31/2023 |
| `ARGUS_RE_EU_DATE` | European date format | `DD/MM/YYYY` | 31/01/2023 |
| `ARGUS_RE_TIME24` | 24-hour time format | `hh:mm[:ss]` | 14:30:45 |

## Phone Numbers

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_PHONE_INTL` | International phone number | `+[country][number]` | +12345678901 |
| `ARGUS_RE_PHONE_US` | North American phone number | `xxx-xxx-xxxx` or `(xxx) xxx-xxxx` | 123-456-7890 |
| `ARGUS_RE_PHONE_EU` | European phone number (general) | `+xx xx xxx xx xx` | +33 1 23 45 67 89 |

## Identity and Security

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_USER` | Username | `3-20 chars: a-z, A-Z, 0-9, _, -` | john_doe-123 |
| `ARGUS_RE_PASSWD` | Simple password | `8+ chars, letters & numbers` | password123 |
| `ARGUS_RE_PASSWD_STRONG` | Strong password | `8+ chars with lowercase, uppercase, digit, symbol` | P@ssw0rd! |
| `ARGUS_RE_UUID` | UUID (version 4) | `xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx` | 123e4567-e89b-42d3-a456-556642440000 |

## Geographic and Localization

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_ZIP` | US ZIP code | `xxxxx` or `xxxxx-xxxx` | 12345-6789 |
| `ARGUS_RE_UK_POST` | UK postcode | `XX[X] X[XX]` | SW1A 1AA |
| `ARGUS_RE_CA_POST` | Canadian postal code | `XnX nXn` | A1A 1A1 |
| `ARGUS_RE_LATITUDE` | Latitude | `-90 to 90 with decimals` | 40.7128 |
| `ARGUS_RE_LONGITUDE` | Longitude | `-180 to 180 with decimals` | -74.0060 |

## Files and Paths

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_UNIX_PATH` | Unix absolute path | `/path/to/file` | /home/user/document.txt |
| `ARGUS_RE_WIN_PATH` | Windows absolute path | `X:\path\to\file` | C:\Users\User\Document.txt |
| `ARGUS_RE_FILENAME` | Filename with extension | `name.ext` | document.txt |

## Numbers and Codes

| Pattern | Description | Format | Example |
|---------|-------------|--------|---------|
| `ARGUS_RE_HEX_COLOR` | Hexadecimal color | `#RRGGBB` or `#RGB` | #FF00FF |
| `ARGUS_RE_RGB` | RGB color | `rgb(r,g,b)` | rgb(255,0,255) |
| `ARGUS_RE_SEMVER` | Semantic version | `X.Y.Z[-pre][+build]` | 1.2.3-alpha+001 |
| `ARGUS_RE_POS_INT` | Positive integer | `1+` | 42 |
| `ARGUS_RE_NEG_INT` | Negative integer | `-1-` | -42 |
| `ARGUS_RE_FLOAT` | Floating-point number | `±n.n` | -3.14 |
| `ARGUS_RE_HEX` | Hexadecimal number | `0xnnnn` | 0x1A3F |

## Custom Patterns

You can define your own regex patterns using the `MAKE_REGEX` macro:

=== "Definition"
    ```c
    #define RE_PRODUCT_ID MAKE_REGEX(
        "^[A-Z]{2}\\d{4}$", 
        "Format: XX0000"
    )
    ```

=== "Usage"
    ```c
    OPTION_STRING('p', "product", HELP("Product ID"), 
                 REGEX(RE_PRODUCT_ID))
    ```

The `MAKE_REGEX` macro takes two parameters:
1. The regular expression pattern
2. A hint or error message displayed when validation fails

!!! note "Escaping in C Strings"
    When writing regex patterns in C code, remember to double-escape special characters:
    
    - Use `\\d` instead of `\d` for digits
    - Use `\\w` instead of `\w` for word characters
    - Use `\\s` instead of `\s` for whitespace

## PCRE2 Features

argus uses PCRE2 (Perl Compatible Regular Expressions) for regex functionality, providing access to advanced pattern matching capabilities:

- Character classes (`\d`, `\w`, `\s`)
- Quantifiers (`*`, `+`, `?`, `{n,m}`)
- Alternation (`a|b`)
- Grouping (`(abc)`)
- Anchors (`^`, `$`)
- Assertions (lookahead, lookbehind)
- Unicode support

!!! tip "Learning Regular Expressions"
    For more information on how to create and use regular expressions, consider these resources:
    
    - [Regular Expressions Info](https://www.regular-expressions.info/)
    - [PCRE2 Documentation](https://www.pcre.org/current/doc/html/)
    - [RegExr: Learn, Build, & Test RegEx](https://regexr.com/)

## Related Documentation

For more information on using regular expressions with argus:

- [Validation Guide](../guide/validation.md) - Basic regex validation
- [Regular Expressions Advanced Guide](../advanced/regex.md) - Detailed regex usage
