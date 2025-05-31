# Regex Patterns

Predefined regex patterns for common validation scenarios.

:::note Regex Dependency
Regex validation requires PCRE2. If compiled without regex support (`-Dregex=false`), these patterns return stub implementations that report "Regex support disabled".
:::

## Usage

```c
#include "argus/regex.h"

OPTION_STRING('e', "email", HELP("Email address"),
              VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))
```

## Network Patterns

### IP Addresses

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_IPV4` | IPv4 addresses | `192.168.1.1` |
| `ARGUS_RE_IP4CIDR` | IPv4 with optional CIDR | `192.168.1.0/24` |
| `ARGUS_RE_IPV6` | IPv6 addresses | `2001:db8::1` |

```c
OPTION_STRING('i', "ip", HELP("Server IP address"),
              VALIDATOR(V_REGEX(ARGUS_RE_IPV4)))

OPTION_STRING('n', "network", HELP("Network range"),
              VALIDATOR(V_REGEX(ARGUS_RE_IP4CIDR)))
```

### Network Infrastructure

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_MAC` | MAC addresses | `01:23:45:67:89:AB` |
| `ARGUS_RE_DOMAIN` | Domain names (FQDN) | `example.com` |
| `ARGUS_RE_PORT` | Port numbers (1-65535) | `8080` |

```c
OPTION_STRING('m', "mac", HELP("MAC address"),
              VALIDATOR(V_REGEX(ARGUS_RE_MAC)))

OPTION_STRING('d', "domain", HELP("Domain name"),
              VALIDATOR(V_REGEX(ARGUS_RE_DOMAIN)))
```

### URLs

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_URL` | Any protocol URL | `https://example.com/path` |
| `ARGUS_RE_HTTP` | HTTP(S) URLs only | `http://example.com` |
| `ARGUS_RE_FILE_URL` | File URLs | `file:///path/to/file` |

```c
OPTION_STRING('u', "url", HELP("Website URL"),
              VALIDATOR(V_REGEX(ARGUS_RE_URL)))

OPTION_STRING('a', "api", HELP("API endpoint"),
              VALIDATOR(V_REGEX(ARGUS_RE_HTTP)))
```

## Communication Patterns

### Email

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_EMAIL` | Basic email format | `user@example.com` |
| `ARGUS_RE_EMAIL_STRICT` | RFC-compliant email | `user.name+tag@example.com` |

```c
OPTION_STRING('e', "email", HELP("Email address"),
              VALIDATOR(V_REGEX(ARGUS_RE_EMAIL)))

OPTION_STRING('c', "contact", HELP("Contact email (strict)"),
              VALIDATOR(V_REGEX(ARGUS_RE_EMAIL_STRICT)))
```

### Phone Numbers

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_PHONE_INTL` | International format | `+12345678901` |
| `ARGUS_RE_PHONE_US` | US phone numbers | `123-456-7890` |
| `ARGUS_RE_PHONE_EU` | European phone numbers | `+33 123456789` |

```c
OPTION_STRING('p', "phone", HELP("Phone number"),
              VALIDATOR(V_REGEX(ARGUS_RE_PHONE_US)))
```

## Date and Time Patterns

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_ISO_DATE` | ISO date (YYYY-MM-DD) | `2024-03-15` |
| `ARGUS_RE_ISOTIME` | ISO datetime | `2024-03-15T14:30:00` |
| `ARGUS_RE_US_DATE` | US date format | `03/15/2024` |
| `ARGUS_RE_EU_DATE` | European date format | `15/03/2024` |
| `ARGUS_RE_TIME24` | 24-hour time | `14:30:00` |

```c
OPTION_STRING('d', "date", HELP("Date (YYYY-MM-DD)"),
              VALIDATOR(V_REGEX(ARGUS_RE_ISO_DATE)))

OPTION_STRING('t', "time", HELP("Time (HH:MM:SS)"),
              VALIDATOR(V_REGEX(ARGUS_RE_TIME24)))
```

## Geographic Patterns

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_ZIP` | US ZIP codes | `12345` or `12345-6789` |
| `ARGUS_RE_UK_POST` | UK postcodes | `SW1A 1AA` |
| `ARGUS_RE_CA_POST` | Canadian postal codes | `A1A 1A1` |
| `ARGUS_RE_LATITUDE` | Latitude (-90 to 90) | `40.7128` |
| `ARGUS_RE_LONGITUDE` | Longitude (-180 to 180) | `-74.0060` |

```c
OPTION_STRING('z', "zip", HELP("ZIP code"),
              VALIDATOR(V_REGEX(ARGUS_RE_ZIP)))

OPTION_STRING('l', "lat", HELP("Latitude"),
              VALIDATOR(V_REGEX(ARGUS_RE_LATITUDE)))
```

## Security Patterns

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_USER` | Username (3-20 chars) | `user_123` |
| `ARGUS_RE_PASSWD` | Basic password | `password123` |
| `ARGUS_RE_PASSWD_STRONG` | Strong password | `Pass123!` |
| `ARGUS_RE_UUID` | UUID v4 format | `550e8400-e29b-41d4-a716-446655440000` |

```c
OPTION_STRING('u', "username", HELP("Username"),
              VALIDATOR(V_REGEX(ARGUS_RE_USER)))

OPTION_STRING('p', "password", HELP("Strong password"),
              VALIDATOR(V_REGEX(ARGUS_RE_PASSWD_STRONG)))
```

## File System Patterns

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_UNIX_PATH` | Unix absolute paths | `/path/to/file` |
| `ARGUS_RE_WIN_PATH` | Windows absolute paths | `C:\folder\file` |
| `ARGUS_RE_FILENAME` | Filename with extension | `document.pdf` |

```c
OPTION_STRING('f', "file", HELP("File path"),
              VALIDATOR(V_REGEX(ARGUS_RE_UNIX_PATH)))

OPTION_STRING('n', "name", HELP("Filename"),
              VALIDATOR(V_REGEX(ARGUS_RE_FILENAME)))
```

## Visual and Data Patterns

### Colors

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_HEX_COLOR` | Hex colors | `#FF0000` or `#F00` |
| `ARGUS_RE_RGB` | RGB color format | `rgb(255,0,0)` |

```c
OPTION_STRING('c', "color", HELP("Hex color"),
              VALIDATOR(V_REGEX(ARGUS_RE_HEX_COLOR)))
```

### Numbers and Formats

| Pattern | Validates | Example |
|---------|-----------|---------|
| `ARGUS_RE_SEMVER` | Semantic version | `1.2.3` or `1.0.0-beta+build` |
| `ARGUS_RE_POS_INT` | Positive integers | `123` |
| `ARGUS_RE_NEG_INT` | Negative integers | `-123` |
| `ARGUS_RE_FLOAT` | Decimal numbers | `3.14` or `-2.7` |
| `ARGUS_RE_HEX` | Hex numbers | `0x1A3F` |

```c
OPTION_STRING('v', "version", HELP("Semantic version"),
              VALIDATOR(V_REGEX(ARGUS_RE_SEMVER)))

OPTION_STRING('h', "hex", HELP("Hex number"),
              VALIDATOR(V_REGEX(ARGUS_RE_HEX)))
```

## Custom Patterns

Define your own patterns using `MAKE_REGEX`:

```c
// Define at file scope
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
#define RE_LICENSE_KEY MAKE_REGEX("^[A-Z0-9]{4}-[A-Z0-9]{4}-[A-Z0-9]{4}$", "Format: XXXX-XXXX-XXXX")

ARGUS_OPTIONS(options,
    OPTION_STRING('p', "product", HELP("Product ID"),
                  VALIDATOR(V_REGEX(RE_PRODUCT_ID))),
    
    OPTION_STRING('k', "key", HELP("License key"),
                  VALIDATOR(V_REGEX(RE_LICENSE_KEY)))
)
```

## Pattern Testing

Test patterns before use:

```bash
# Valid examples
./app --email user@example.com          # ✅ Matches ARGUS_RE_EMAIL
./app --ip 192.168.1.1                  # ✅ Matches ARGUS_RE_IPV4  
./app --date 2024-03-15                  # ✅ Matches ARGUS_RE_ISO_DATE

# Invalid examples  
./app --email invalid-email             # ❌ Error: Invalid value 'invalid-email': Enter email: user@example.com
./app --ip 999.999.999.999              # ❌ Error: Invalid value '999.999.999.999': Enter valid IPv4: 192.168.1.1
./app --date 03-15-2024                  # ❌ Error: Invalid value '03-15-2024': Enter date: YYYY-MM-DD
```

## Error Messages

Each pattern includes helpful error hints:

- `ARGUS_RE_EMAIL`: "Enter email: user@example.com"
- `ARGUS_RE_IPV4`: "Enter valid IPv4: 192.168.1.1"  
- `ARGUS_RE_PHONE_US`: "Enter US phone: 123-456-7890"
- `ARGUS_RE_ISO_DATE`: "Enter date: YYYY-MM-DD"

Custom patterns should include similar hints:

```c
#define RE_CUSTOM MAKE_REGEX("pattern", "Clear format description")
```

## Performance Notes

- Patterns are compiled once at first use
- Complex patterns (like IPv6) may impact startup time
- Use simpler validation when regex isn't needed

## What's Next?

- **[Custom Validators](../../advanced/custom-validators)** - Create domain-specific validation
- **[Validation](../../features/validation)** - Combine regex with other validators
- **[Examples](../../examples/simple-cli)** - See patterns in real applications
