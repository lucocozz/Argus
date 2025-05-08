#include <stddef.h>

#include "argus/internal/compiler.h"

#ifdef ARGUS_REGEX
    #define PCRE2_CODE_UNIT_WIDTH 8
    
    /* Inclusion conditionnelle selon la plateforme */
    #if ARGUS_PLATFORM_WINDOWS
        /* Essayer plusieurs chemins possibles pour Windows */
        #if defined(_MSC_VER)
            #pragma warning(push)
            #pragma warning(disable:4996)  /* Ignorer l'avertissement d'inclusion en minuscules */
        #endif
        
        /* Tenter différentes possibilités de chemins */
        #if __has_include(<pcre2.h>)
            #include <pcre2.h>
        #elif __has_include("pcre2/pcre2.h")
            #include "pcre2/pcre2.h"
        #elif __has_include("include/pcre2.h")
            #include "include/pcre2.h"
        #elif __has_include("pcre2.h")
            #include "pcre2.h"
        #else
            #error "PCRE2 headers not found! Please install PCRE2 development files or disable regex support."
        #endif
        
        #if defined(_MSC_VER)
            #pragma warning(pop)
        #endif
    #else
        /* Sur les autres plateformes, utiliser l'inclusion standard */
        #include <pcre2.h>
    #endif
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
#ifndef ARGUS_REGEX
    (void)(value);
    (void)(data);
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "regex support is not available");
    return ARGUS_ERROR_INVALID_VALUE;
#else
    const char *pattern = data.regex.pattern;
    if (!pattern) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE, "Regular expression pattern is NULL");
    }

    int         errorcode;
    PCRE2_SIZE  erroroffset;
    pcre2_code *re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
                                   &erroroffset, NULL);

    if (re == NULL) {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT, "Failed to compile regex '%s': %s",
                           pattern, buffer);
    }

    pcre2_match_data *match_data = pcre2_match_data_create_from_pattern(re, NULL);
    int rc = pcre2_match(re, (PCRE2_SPTR)value, PCRE2_ZERO_TERMINATED, 0, 0, match_data, NULL);

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
