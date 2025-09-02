/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_REGEX_H
#define ARGUS_REGEX_H

#ifndef ARGUS_REGEX

    // Stub implementation when regex support is disabled
    #define MAKE_REGEX(_pattern, _hint)                                                            \
        (regex_data_t)                                                                             \
        {                                                                                          \
            .pattern = _pattern, .hint = _hint                                                     \
        }

    /*
     * Network related patterns
     */

    /* IPv4 address: 0-255.0-255.0-255.0-255 */
    #define ARGUS_RE_IPV4 MAKE_REGEX("", "Regex support disabled")
    /* Simplified IPv6 pattern */
    #define ARGUS_RE_IPV6 MAKE_REGEX("", "Regex support disabled")
    /* FQDN (Fully Qualified Domain Name) */
    #define ARGUS_RE_DOMAIN MAKE_REGEX("", "Regex support disabled")
    /* URL pattern - supports many protocols */
    #define ARGUS_RE_URL MAKE_REGEX("", "Regex support disabled")
    /* More specific URL patterns */
    #define ARGUS_RE_HTTP MAKE_REGEX("", "Regex support disabled")
    /* Basic port number (1-65535) */
    #define ARGUS_RE_PORT MAKE_REGEX("", "Regex support disabled")
    /*
     * Email related patterns
     */

    /* Basic email validation */
    #define ARGUS_RE_EMAIL MAKE_REGEX("", "Regex support disabled")
    /* More strict email validation */
    #define ARGUS_RE_EMAIL_STRICT MAKE_REGEX("", "Regex support disabled")
    /*
     * Date and time patterns
     */

    /* ISO 8601 date (YYYY-MM-DD) */
    #define ARGUS_RE_ISO_DATE MAKE_REGEX("", "Regex support disabled")
    /* ISO 8601 datetime (YYYY-MM-DDThh:mm:ss) */
    #define ARGUS_RE_ISOTIME MAKE_REGEX("", "Regex support disabled")
    /*
     * Identity and security patterns
     */

    /* Username: 3-20 characters, alphanumeric with underscores and hyphens */
    #define ARGUS_RE_USER MAKE_REGEX("", "Regex support disabled")
    /* Simple password (8+ chars, at least one letter and one number) */
    #define ARGUS_RE_PASSWD MAKE_REGEX("", "Regex support disabled")
    /* UUID (version 4) */
    #define ARGUS_RE_UUID MAKE_REGEX("", "Regex support disabled")
    /*
     * File and path patterns
     */

    /* Unix absolute path */
    #define ARGUS_RE_UNIX_PATH MAKE_REGEX("", "Regex support disabled")
    /* File name with extension */
    #define ARGUS_RE_FILENAME MAKE_REGEX("", "Regex support disabled")
    /*
     * Numbers and code patterns
     */
    /* Semantic version (major.minor.patch) */
    #define ARGUS_RE_SEMVER MAKE_REGEX("", "Regex support disabled")
    /* Positive integer */
    #define ARGUS_RE_POS_INT MAKE_REGEX("", "Regex support disabled")
    /* Negative integer */
    #define ARGUS_RE_NEG_INT MAKE_REGEX("", "Regex support disabled")
    /* Float (positive or negative with decimal places) */
    #define ARGUS_RE_FLOAT MAKE_REGEX("", "Regex support disabled")
    /* Hex number with 0x prefix */
    #define ARGUS_RE_HEX MAKE_REGEX("", "Regex support disabled")

#else

    /*
     * Network related patterns
     */

    // clang-format off

#define MAKE_REGEX(_pattern, _hint) (regex_data_t){ .pattern = _pattern, .hint = _hint }
    // clang-format on

    /* IPv4 address: 0-255.0-255.0-255.0-255 */
    #define ARGUS_RE_IPV4                                                                          \
        MAKE_REGEX("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-"  \
                   "9][0-9]?)",                                                                    \
                   "IPv4")

    /* Simplified IPv6 pattern */
    #define ARGUS_RE_IPV6                                                                          \
        MAKE_REGEX("^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-"  \
                   "fA-F]{1,4}:)"                                                                  \
                   "{1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-"  \
                   "9a-fA-F]{1,"                                                                   \
                   "4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4})" \
                   "{1,4}|([0-"                                                                    \
                   "9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{" \
                   "1,4}){1,6})"                                                                   \
                   "|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|" \
                   "::(ffff(:0{"                                                                   \
                   "1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\\.){3,3}(25[0-5]|(" \
                   "2[0-4]|1{0,"                                                                   \
                   "1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){"  \
                   "0,1}[0-9])"                                                                    \
                   "\\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))$",                          \
                   "IPv6")

    /* FQDN (Fully Qualified Domain Name) */
    #define ARGUS_RE_DOMAIN                                                                        \
        MAKE_REGEX("^(?=.{1,253}$)((?!-)[A-Za-z0-9-]{1,63}(?<!-)\\.)+[A-Za-z]{2,}$", "domain")

    /* URL pattern - supports many protocols */
    #define ARGUS_RE_URL  MAKE_REGEX("^([a-zA-Z][a-zA-Z0-9+.-]*):\\/\\/[^\\s/$.?#].[^\\s]*$", "URL")

    /* More specific URL patterns */
    #define ARGUS_RE_HTTP MAKE_REGEX("^https?:\\/\\/[^\\s/$.?#].[^\\s]*$", "HTTP-URL")

    /* Basic port number (1-65535) */
    #define ARGUS_RE_PORT                                                                          \
        MAKE_REGEX("^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|" \
                   "6553[0-5])$",                                                                  \
                   "port")

    /*
     * Email related patterns
     */

    /* Basic email validation */
    #define ARGUS_RE_EMAIL MAKE_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "email")

    /* More strict email validation */
    #define ARGUS_RE_EMAIL_STRICT                                                                  \
        MAKE_REGEX(                                                                                \
            "^[a-zA-Z0-9.!#$%&'*+/"                                                                \
            "=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-"    \
            "zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$",                                                     \
            "email-strict")

    /*
     * Date and time patterns
     */

    /* ISO 8601 date (YYYY-MM-DD) */
    #define ARGUS_RE_ISO_DATE                                                                      \
        MAKE_REGEX("^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])$", "YYYY-MM-DD")

    /* ISO 8601 datetime (YYYY-MM-DDThh:mm:ss) */
    #define ARGUS_RE_ISOTIME                                                                       \
        MAKE_REGEX(                                                                                \
            "^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])T([0][0-9]|[1][0-9]|[2]"    \
            "[0-3]):([0-5][0-9]):([0-5][0-9])$",                                                   \
            "ISO-datetime")

    /*
     * Identity and security patterns
     */

    /* Username: 3-20 characters, alphanumeric with underscores and hyphens */
    #define ARGUS_RE_USER   MAKE_REGEX("^[a-zA-Z0-9_-]{3,20}$", "username")

    /* Simple password (8+ chars, at least one letter and one number) */
    #define ARGUS_RE_PASSWD MAKE_REGEX("^(?=.*[A-Za-z])(?=.*\\d)[A-Za-z\\d]{8,}$", "password")

    /* UUID (version 4) */
    #define ARGUS_RE_UUID                                                                          \
        MAKE_REGEX("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-4[0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-"  \
                   "fA-F]{12}$",                                                                   \
                   "UUID")

    /*
     * File and path patterns
     */

    /* Unix absolute path */
    #define ARGUS_RE_UNIX_PATH MAKE_REGEX("^(/[^/ ]*)+/?$", "unix-path")

    /* File name with extension */
    #define ARGUS_RE_FILENAME  MAKE_REGEX("^[\\w,\\s-]+\\.[A-Za-z]{1,5}$", "filename")

    /*
     * Numbers and code patterns
     */

    /* Semantic version (major.minor.patch) */
    #define ARGUS_RE_SEMVER                                                                        \
        MAKE_REGEX(                                                                                \
            "^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-]["     \
            "0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\\+([0-9a-"      \
            "zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$",                                                    \
            "semver")

    /* Positive integer */
    #define ARGUS_RE_POS_INT MAKE_REGEX("^[1-9][0-9]*$", "positive-int")

    /* Negative integer */
    #define ARGUS_RE_NEG_INT MAKE_REGEX("^-[1-9][0-9]*$", "negative-int")

    /* Float (positive or negative with decimal places) */
    #define ARGUS_RE_FLOAT   MAKE_REGEX("^[-+]?[0-9]*\\.?[0-9]+$", "decimal")

    /* Hex number with 0x prefix */
    #define ARGUS_RE_HEX     MAKE_REGEX("^0x[0-9a-fA-F]+$", "hex")

#endif /* ARGUS_REGEX */

/*
 * Convenience validator macros for common patterns
 * These can be used directly with VALIDATOR() without V_REGEX()
 */

/* Network validators */

/* IPv4 address: 0-255.0-255.0-255.0-255 */
#define V_RE_IPV4() V_REGEX(ARGUS_RE_IPV4)
/* Simplified IPv6 pattern */
#define V_RE_IPV6() V_REGEX(ARGUS_RE_IPV6)
/* FQDN (Fully Qualified Domain Name) */
#define V_RE_DOMAIN() V_REGEX(ARGUS_RE_DOMAIN)
/* URL pattern - supports many protocols */
#define V_RE_URL() V_REGEX(ARGUS_RE_URL)
/* More specific URL patterns */
#define V_RE_HTTP() V_REGEX(ARGUS_RE_HTTP)
/* Basic port number (1-65535) */
#define V_RE_PORT() V_REGEX(ARGUS_RE_PORT)

/* Email validators */

/* Basic email validation */
#define V_RE_EMAIL() V_REGEX(ARGUS_RE_EMAIL)
/* More strict email validation */
#define V_RE_EMAIL_STRICT() V_REGEX(ARGUS_RE_EMAIL_STRICT)

/* Date and time validators */
/* ISO 8601 date (YYYY-MM-DD) */
#define V_RE_ISO_DATE() V_REGEX(ARGUS_RE_ISO_DATE)
/* ISO 8601 datetime (YYYY-MM-DDThh:mm:ss) */
#define V_RE_ISO_TIME() V_REGEX(ARGUS_RE_ISOTIME)

/* Identity and security validators */
/* Username: 3-20 characters, alphanumeric with underscores and hyphens */
#define V_RE_USERNAME() V_REGEX(ARGUS_RE_USER)
/* Simple password (8+ chars, at least one letter and one number) */
#define V_RE_PASSWORD() V_REGEX(ARGUS_RE_PASSWD)
/* UUID (version 4) */
#define V_RE_UUID() V_REGEX(ARGUS_RE_UUID)

/* File and path validators */
/* Unix absolute path */
#define V_RE_UNIX_PATH() V_REGEX(ARGUS_RE_UNIX_PATH)
/* File name with extension */
#define V_RE_FILENAME() V_REGEX(ARGUS_RE_FILENAME)

/* Number validators */
/* Semantic version (major.minor.patch) */
#define V_RE_SEMVER() V_REGEX(ARGUS_RE_SEMVER)
/* Positive integer */
#define V_RE_POS_INT() V_REGEX(ARGUS_RE_POS_INT)
/* Negative integer */
#define V_RE_NEG_INT() V_REGEX(ARGUS_RE_NEG_INT)
/* Float (positive or negative with decimal places) */
#define V_RE_FLOAT() V_REGEX(ARGUS_RE_FLOAT)
/* Hex number with 0x prefix */
#define V_RE_HEX() V_REGEX(ARGUS_RE_HEX)

#endif /* ARGUS_REGEX_H */
