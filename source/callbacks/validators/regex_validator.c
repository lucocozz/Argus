#include <stddef.h>
#ifndef ARGUS_NO_REGEX
    #define PCRE2_CODE_UNIT_WIDTH 8
    #include <pcre2.h>
#endif
#include "argus/errors.h"
#include "argus/types.h"

/**
 * regex_validator - Validate a string value against a regular expression
 *
 * @param argus  Argus context
 * @param value  String value to validate
 * @param data   Validator data containing regex pattern
 *
 * @return Status code (0 for success, non-zero for error)
 */
int regex_validator(argus_t *argus, const char *value, validator_data_t data)
{
#ifdef ARGUS_NO_REGEX
    // Regex support is disabled
    (void)(value);
    (void)(data);
    ARGUS_REPORT_ERROR(
        argus, ARGUS_ERROR_INVALID_VALUE,
        "Regex validation is not supported in this build (compiled with ARGUS_NO_REGEX)");
    return ARGUS_ERROR_INVALID_VALUE;
#else
    const char *pattern = data.regex.pattern;
    if (!pattern) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Regular expression pattern is NULL");
    }

    // Compile the regular expression
    int         errorcode;
    PCRE2_SIZE  erroroffset;
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
                                   &erroroffset, NULL);

    if (re == NULL) {
        // Failed to compile the regex
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Failed to compile regex '%s': %s",
                           pattern, buffer);
    }

    // Execute the regex against the input string
    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)value, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);

    // Free resources
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                if (data.regex.hint && data.regex.hint[0] != '\0') {
                    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Invalid value '%s': %s",
                                       value, data.regex.hint);
                } else {
                    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                       "Value '%s' does not match the expected format", value);
                }
            default:
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                                   "Internal error: Regex match failed with error code %d", rc);
        }
    }
    return (ARGUS_SUCCESS);
#endif
}
