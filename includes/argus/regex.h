/**
 * argus/patterns.h - Common regex patterns
 *
 * This header provides a collection of common regex patterns that can be used
 * with the REGEX validator to validate option values.
 *
 * MIT License - Copyright (c) 2024 lucocozz
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

    // Define stubs for all regex patterns
    #define ARGUS_RE_IPV4          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_IP4CIDR       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_IPV6          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_MAC           MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_DOMAIN        MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_URL           MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_HTTP          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_FILE_URL      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PORT          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_EMAIL         MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_EMAIL_STRICT  MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_ISO_DATE      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_ISOTIME       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_US_DATE       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_EU_DATE       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_TIME24        MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PHONE_INTL    MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PHONE_US      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PHONE_EU      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_USER          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PASSWD        MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_PASSWD_STRONG MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_UUID          MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_ZIP           MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_UK_POST       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_CA_POST       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_LATITUDE      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_LONGITUDE     MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_UNIX_PATH     MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_WIN_PATH      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_FILENAME      MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_HEX_COLOR     MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_RGB           MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_SEMVER        MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_POS_INT       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_NEG_INT       MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_FLOAT         MAKE_REGEX("", "Regex support disabled")
    #define ARGUS_RE_HEX           MAKE_REGEX("", "Regex support disabled")

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

    /* IPv4 with optional CIDR suffix: 192.168.1.0/24 */
    #define ARGUS_RE_IP4CIDR                                                                       \
        MAKE_REGEX(                                                                                \
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-"    \
            "9]?)(\\/([0-9]|[1-2][0-9]|3[0-2]))?$",                                                \
            "IPv4/CIDR")

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

    /* MAC address (with : or - separators) */
    #define ARGUS_RE_MAC                                                                           \
        MAKE_REGEX("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", "MAC")

    /* FQDN (Fully Qualified Domain Name) */
    #define ARGUS_RE_DOMAIN                                                                        \
        MAKE_REGEX("^(?=.{1,253}$)((?!-)[A-Za-z0-9-]{1,63}(?<!-)\\.)+[A-Za-z]{2,}$",               \
                   "domain")

    /* URL pattern - supports many protocols */
    #define ARGUS_RE_URL                                                                           \
        MAKE_REGEX("^([a-zA-Z][a-zA-Z0-9+.-]*):\\/\\/[^\\s/$.?#].[^\\s]*$",                        \
                   "URL")

    /* More specific URL patterns */
    #define ARGUS_RE_HTTP                                                                          \
        MAKE_REGEX("^https?:\\/\\/[^\\s/$.?#].[^\\s]*$", "HTTP-URL")

    #define ARGUS_RE_FILE_URL                                                                      \
        MAKE_REGEX("^file:\\/\\/[^\\s/$.?#].[^\\s]*$", "file-URL")

    /* Basic port number (1-65535) */
    #define ARGUS_RE_PORT                                                                          \
        MAKE_REGEX("^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|" \
                   "6553[0-5])$",                                                                  \
                   "port")

    /*
     * Email related patterns
     */

    /* Basic email validation */
    #define ARGUS_RE_EMAIL                                                                         \
        MAKE_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "email")

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
        MAKE_REGEX("^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])$",                  \
                   "YYYY-MM-DD")

    /* ISO 8601 datetime (YYYY-MM-DDThh:mm:ss) */
    #define ARGUS_RE_ISOTIME                                                                       \
        MAKE_REGEX(                                                                                \
            "^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])T([0][0-9]|[1][0-9]|[2]"    \
            "[0-3]):([0-5][0-9]):([0-5][0-9])$",                                                   \
            "ISO-datetime")

    /* MM/DD/YYYY format */
    #define ARGUS_RE_US_DATE                                                                       \
        MAKE_REGEX("^(0[1-9]|1[0-2])\\/(0[1-9]|[12][0-9]|3[01])\\/\\d{4}$", "MM/DD/YYYY")

    /* DD/MM/YYYY format */
    #define ARGUS_RE_EU_DATE                                                                       \
        MAKE_REGEX("^(0[1-9]|[12][0-9]|3[01])\\/(0[1-9]|1[0-2])\\/\\d{4}$", "DD/MM/YYYY")

    /* Time (24h format) */
    #define ARGUS_RE_TIME24                                                                        \
        MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9](:[0-5][0-9])?$", "hh:mm:ss")

    /*
     * Phone number patterns
     */

    /* International phone number with optional country code */
    #define ARGUS_RE_PHONE_INTL                                                                    \
        MAKE_REGEX("^\\+?[1-9]\\d{1,14}$", "phone-intl")

    /* North American phone number: 123-456-7890 or (123) 456-7890 */
    #define ARGUS_RE_PHONE_US                                                                      \
        MAKE_REGEX("^(\\+?1[-\\s]?)?(\\([0-9]{3}\\)|[0-9]{3})[-\\s]?[0-9]{3}[-\\s]?[0-9]{4}$",     \
                   "phone-US")

    /* European phone number (general pattern) */
    #define ARGUS_RE_PHONE_EU                                                                      \
        MAKE_REGEX("^\\+?[0-9]{1,3}[-\\s]?[0-9]{2,3}[-\\s]?[0-9]{2,3}[-\\s]?[0-9]{2,3}$",          \
                   "phone-EU")

    /*
     * Identity and security patterns
     */

    /* Username: 3-20 characters, alphanumeric with underscores and hyphens */
    #define ARGUS_RE_USER                                                                          \
        MAKE_REGEX("^[a-zA-Z0-9_-]{3,20}$", "username")

    /* Simple password (8+ chars, at least one letter and one number) */
    #define ARGUS_RE_PASSWD                                                                        \
        MAKE_REGEX("^(?=.*[A-Za-z])(?=.*\\d)[A-Za-z\\d]{8,}$",                                     \
                   "password")

    /* Strong password (8+ chars with lowercase, uppercase, number, special char) */
    #define ARGUS_RE_PASSWD_STRONG                                                                 \
        MAKE_REGEX("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$",       \
                   "secure-password")

    /* UUID (version 4) */
    #define ARGUS_RE_UUID                                                                          \
        MAKE_REGEX("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-4[0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-"  \
                   "fA-F]{12}$",                                                                   \
                   "UUID")

    /*
     * Geographic and localization patterns
     */

    /* US Zip Code (5 digits, optional 4 digit extension) */
    #define ARGUS_RE_ZIP MAKE_REGEX("^[0-9]{5}(?:-[0-9]{4})?$", "ZIP")

    /* UK Postcode */
    #define ARGUS_RE_UK_POST                                                                       \
        MAKE_REGEX("^[A-Z]{1,2}[0-9][A-Z0-9]? ?[0-9][A-Z]{2}$", "UK-postcode")

    /* Canadian Postal Code */
    #define ARGUS_RE_CA_POST                                                                       \
        MAKE_REGEX("^[A-Za-z][0-9][A-Za-z] ?[0-9][A-Za-z][0-9]$", "CA-postcode")

    /* Latitude (-90 to 90) with decimal precision */
    #define ARGUS_RE_LATITUDE                                                                      \
        MAKE_REGEX("^[-+]?([1-8]?\\d(\\.\\d+)?|90(\\.0+)?)$", "latitude")

    /* Longitude (-180 to 180) with decimal precision */
    #define ARGUS_RE_LONGITUDE                                                                     \
        MAKE_REGEX("^[-+]?(180(\\.0+)?|((1[0-7]\\d)|([1-9]?\\d))(\\.\\d+)?)$",                     \
                   "longitude")

    /*
     * File and path patterns
     */

    /* Unix absolute path */
    #define ARGUS_RE_UNIX_PATH MAKE_REGEX("^(/[^/ ]*)+/?$", "unix-path")

    /* Windows absolute path (with drive letter) */
    #define ARGUS_RE_WIN_PATH                                                                      \
        MAKE_REGEX("^[a-zA-Z]:\\\\([^\\\\/:*?\"<>|]+(\\\\)?)*$", "win-path")

    /* File name with extension */
    #define ARGUS_RE_FILENAME                                                                      \
        MAKE_REGEX("^[\\w,\\s-]+\\.[A-Za-z]{1,5}$", "filename")

    /*
     * Numbers and code patterns
     */

    /* Hexadecimal color (e.g. #FF00FF) */
    #define ARGUS_RE_HEX_COLOR                                                                     \
        MAKE_REGEX("^#?([a-fA-F0-9]{6}|[a-fA-F0-9]{3})$", "hex-color")

    /* RGB color (e.g. rgb(255,0,255)) */
    #define ARGUS_RE_RGB                                                                           \
        MAKE_REGEX("^rgb\\(\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*\\)$",          \
                   "RGB")

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

/*
 * Convenience validator macros for common patterns
 * These can be used directly with VALIDATOR() without V_REGEX()
 */

/* Network validators */
#define V_RE_IPV4()        V_REGEX(ARGUS_RE_IPV4)
#define V_RE_IPV4_CIDR()   V_REGEX(ARGUS_RE_IP4CIDR)
#define V_RE_IPV6()        V_REGEX(ARGUS_RE_IPV6)
#define V_RE_MAC()         V_REGEX(ARGUS_RE_MAC)
#define V_RE_DOMAIN()      V_REGEX(ARGUS_RE_DOMAIN)
#define V_RE_URL()         V_REGEX(ARGUS_RE_URL)
#define V_RE_HTTP()        V_REGEX(ARGUS_RE_HTTP)
#define V_RE_FILE_URL()    V_REGEX(ARGUS_RE_FILE_URL)
#define V_RE_PORT()        V_REGEX(ARGUS_RE_PORT)

/* Email validators */
#define V_RE_EMAIL()       V_REGEX(ARGUS_RE_EMAIL)
#define V_RE_EMAIL_STRICT() V_REGEX(ARGUS_RE_EMAIL_STRICT)

/* Date and time validators */
#define V_RE_ISO_DATE()    V_REGEX(ARGUS_RE_ISO_DATE)
#define V_RE_ISO_TIME()    V_REGEX(ARGUS_RE_ISOTIME)
#define V_RE_US_DATE()     V_REGEX(ARGUS_RE_US_DATE)
#define V_RE_EU_DATE()     V_REGEX(ARGUS_RE_EU_DATE)
#define V_RE_TIME24()      V_REGEX(ARGUS_RE_TIME24)

/* Phone validators */
#define V_RE_PHONE_INTL()  V_REGEX(ARGUS_RE_PHONE_INTL)
#define V_RE_PHONE_US()    V_REGEX(ARGUS_RE_PHONE_US)
#define V_RE_PHONE_EU()    V_REGEX(ARGUS_RE_PHONE_EU)

/* Identity and security validators */
#define V_RE_USERNAME()    V_REGEX(ARGUS_RE_USER)
#define V_RE_PASSWORD()    V_REGEX(ARGUS_RE_PASSWD)
#define V_RE_PASSWORD_STRONG() V_REGEX(ARGUS_RE_PASSWD_STRONG)
#define V_RE_UUID()        V_REGEX(ARGUS_RE_UUID)

/* Geographic validators */
#define V_RE_ZIP()         V_REGEX(ARGUS_RE_ZIP)
#define V_RE_UK_POST()     V_REGEX(ARGUS_RE_UK_POST)
#define V_RE_CA_POST()     V_REGEX(ARGUS_RE_CA_POST)
#define V_RE_LATITUDE()    V_REGEX(ARGUS_RE_LATITUDE)
#define V_RE_LONGITUDE()   V_REGEX(ARGUS_RE_LONGITUDE)

/* File and path validators */
#define V_RE_UNIX_PATH()   V_REGEX(ARGUS_RE_UNIX_PATH)
#define V_RE_WIN_PATH()    V_REGEX(ARGUS_RE_WIN_PATH)
#define V_RE_FILENAME()    V_REGEX(ARGUS_RE_FILENAME)

/* Number and color validators */
#define V_RE_HEX_COLOR()   V_REGEX(ARGUS_RE_HEX_COLOR)
#define V_RE_RGB()         V_REGEX(ARGUS_RE_RGB)
#define V_RE_SEMVER()      V_REGEX(ARGUS_RE_SEMVER)
#define V_RE_POS_INT()     V_REGEX(ARGUS_RE_POS_INT)
#define V_RE_NEG_INT()     V_REGEX(ARGUS_RE_NEG_INT)
#define V_RE_FLOAT()       V_REGEX(ARGUS_RE_FLOAT)
#define V_RE_HEX()         V_REGEX(ARGUS_RE_HEX)

#endif /* ARGUS_REGEX */

#endif /* ARGUS_REGEX_H */