/**
 * Example demonstrating regex validation for argus
 *
 * Shows a mix of predefined patterns and custom patterns
 */

#include "argus.h"
#include "argus/regex.h"  /* Import predefined patterns */
#include <stdio.h>
#include <stdlib.h>


/* Custom regex patterns */
#define RE_PRODUCT_ID MAKE_REGEX("^[A-Z]{2}\\d{4}-[A-Z0-9]{6}$", "Format: XX0000-XXXXXX")
#define RE_SIMPLE_NAME MAKE_REGEX("^[a-zA-Z][a-zA-Z0-9_-]{2,29}$", "Letters, numbers, underscore, dash")

ARGUS_OPTIONS(
    options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),

    // SECTION 1: Using predefined patterns from header
    GROUP_START("Network and Communication", GROUP_DESC("Network-related options")),
       OPTION_STRING('i', "ip", HELP("IPv4 address"),
                    REGEX(ARGUS_RE_IPV4)),

       OPTION_STRING('m', "mac", HELP("MAC address"),
                    REGEX(ARGUS_RE_MAC)),

        OPTION_STRING('e', "email", HELP("Email address"),
                    REGEX(ARGUS_RE_EMAIL)),

        OPTION_STRING('u', "url", HELP("URL with any protocol"),
                    REGEX(ARGUS_RE_URL)),
    GROUP_END(),

    // SECTION 2: Custom patterns defined in this file
    GROUP_START("Custom Formats", GROUP_DESC("Options with custom regex patterns")),
        OPTION_STRING('p', "product", HELP("Product ID (format: XX0000-XXXXXX)"),
                    REGEX(RE_PRODUCT_ID)),

        OPTION_STRING('n', "name", HELP("Username (letters, numbers, underscore, dash)"),
                    REGEX(RE_SIMPLE_NAME)),
    GROUP_END(),

    // SECTION 3: Custom inline regex (without defining a constant)
    GROUP_START("Inline Patterns", GROUP_DESC("Options with inline regex patterns")),
        OPTION_STRING('z', "zipcode", HELP("US Zip code"),
                    REGEX(MAKE_REGEX("^\\d{5}(-\\d{4})?$", "Format: 12345 or 12345-6789"))),

        OPTION_STRING('t', "time", HELP("Time (format: HH:MM)"),
                    REGEX(MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9]$", "Format: HH:MM"))),

        OPTION_STRING('f', "float", HELP("Floating point number"),
                    REGEX(MAKE_REGEX("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$",
                           "Format: 123.45 or 1.23e-4"))),
    GROUP_END(),

    // SECTION 4: Combining predefined patterns with custom options
    GROUP_START("Combined Patterns", GROUP_DESC("Options with combined validation")),
        // Date with custom error message
        OPTION_STRING('d', "date", HELP("Date (YYYY-MM-DD)"),
                    REGEX(ARGUS_RE_ISO_DATE),
                    HINT("YYYY-MM-DD")),

        // Password with predefined pattern and custom error message
        OPTION_STRING('P', "password", HELP("Password (8+ chars, mixed case, numbers, symbols)"),
                    REGEX(ARGUS_RE_PASSWD_STRONG),
                    HINT("StrongP@ss1")),

        // Version number with additional flags
        OPTION_STRING('v', "version", HELP("Semantic version"),
                    REGEX(ARGUS_RE_SEMVER),
                    HINT("X.Y.Z"),
                    FLAGS(FLAG_REQUIRED)),
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

    // Display validated values grouped by category
    printf("Network & Communication:\n");
    printf("  IP Address: %s\n", argus_is_set(argus, "ip") ?
           argus_get(argus, "ip").as_string : "(not provided)");
    printf("  MAC Address: %s\n", argus_is_set(argus, "mac") ?
           argus_get(argus, "mac").as_string : "(not provided)");
    printf("  Email: %s\n", argus_is_set(argus, "email") ?
           argus_get(argus, "email").as_string : "(not provided)");
    printf("  URL: %s\n", argus_is_set(argus, "url") ?
           argus_get(argus, "url").as_string : "(not provided)");

    printf("\nCustom Formats:\n");
    printf("  Product ID: %s\n", argus_is_set(argus, "product") ?
           argus_get(argus, "product").as_string : "(not provided)");
    printf("  RGB Color: %s\n", argus_is_set(argus, "color") ?
           argus_get(argus, "color").as_string : "(not provided)");
    printf("  Username: %s\n", argus_is_set(argus, "name") ?
           argus_get(argus, "name").as_string : "(not provided)");

    printf("\nInline Patterns:\n");
    printf("  Zip Code: %s\n", argus_is_set(argus, "zipcode") ?
           argus_get(argus, "zipcode").as_string : "(not provided)");
    printf("  Time: %s\n", argus_is_set(argus, "time") ?
           argus_get(argus, "time").as_string : "(not provided)");
    printf("  Float: %s\n", argus_is_set(argus, "float") ?
           argus_get(argus, "float").as_string : "(not provided)");

    printf("\nCombined Patterns:\n");
    printf("  Date: %s\n", argus_is_set(argus, "date") ?
           argus_get(argus, "date").as_string : "(not provided)");
    printf("  Password: %s\n", argus_is_set(argus, "password") ?
           "[HIDDEN]" : "(not provided)");
    printf("  Version: %s\n", argus_get(argus, "version").as_string);

    argus_free(&argus);
    return 0;
}
