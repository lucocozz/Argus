#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef ARGUS_REGEX
    #define PCRE2_CODE_UNIT_WIDTH 8
    #include <pcre2.h>
#endif
#include "argus/errors.h"
#include "argus/internal/cross_platform.h"
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
int regex_validator(argus_t *argus, void *value_ptr, validator_data_t data)
{
#ifndef ARGUS_REGEX
    (void)(value_ptr);
    (void)(data);
    ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "regex support is not available");
    return ARGUS_ERROR_INVALID_VALUE;
#else
    const char *value   = (const char *)value_ptr;
    const char *pattern = data.regex.pattern;
    if (!pattern) {
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Regular expression pattern is NULL");
        return ARGUS_ERROR_INVALID_VALUE;
    }

    int         errorcode;
    PCRE2_SIZE  erroroffset;
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
                                   &erroroffset, NULL);

    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Failed to compile regex '%s': %s",
                            pattern, buffer);
        return ARGUS_ERROR_INVALID_FORMAT;
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)value, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);

    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    if (rc < 0) {
        switch (rc) {
            case PCRE2_ERROR_NOMATCH:
                if (data.regex.hint && data.regex.hint[0] != '\0') {
                    ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                        "Value '%s' does not match required pattern: %s", value,
                                        data.regex.hint);
                } else {
                    ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                        "Value '%s' does not match the expected format", value);
                }
                return ARGUS_ERROR_INVALID_VALUE;
            default:
                ARGUS_PARSING_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                                    "Internal error: Regex match failed with error code %d", rc);
                return ARGUS_ERROR_INVALID_FORMAT;
        }
    }
    return (ARGUS_SUCCESS);
#endif
}

char *format_regex_validator(validator_data_t data)
{
    const char *pattern = data.regex.pattern;
    const char *hint    = data.regex.hint;

    // Priority 1: Use hint if available (remove arbitrary length restriction)
    if (hint && hint[0] != '\0') {
        size_t hint_len = strlen(hint);
        char  *result   = malloc(hint_len + 1);
        if (result)
            safe_strcpy(result, hint_len + 1, hint);
        return result;
    }

    // Priority 2: Use pattern directly if short enough
    if (pattern && pattern[0] != '\0') {
        size_t pattern_len = strlen(pattern);
        if (pattern_len <= 15) {
            char *result = malloc(pattern_len + 1);
            if (result)
                safe_strcpy(result, pattern_len + 1, pattern);
            return result;
        }
    }

    // Priority 3: Fallback to generic pattern indicator
    return safe_strdup("pattern");
}
