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
                   "Enter valid IPv4: 192.168.1.1")

    /* IPv4 with optional CIDR suffix: 192.168.1.0/24 */
    #define ARGUS_RE_IP4CIDR                                                                       \
        MAKE_REGEX(                                                                                \
            "^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-"    \
            "9]?)(\\/([0-9]|[1-2][0-9]|3[0-2]))?$",                                                \
            "Enter IPv4/CIDR: 192.168.1.0/24")

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
                   "Enter valid IPv6: 2001:db8::1")

    /* MAC address (with : or - separators) */
    #define ARGUS_RE_MAC                                                                           \
        MAKE_REGEX("^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})$", "Enter MAC: 01:23:45:67:89:AB")

    /* FQDN (Fully Qualified Domain Name) */
    #define ARGUS_RE_DOMAIN                                                                        \
        MAKE_REGEX("^(?=.{1,253}$)((?!-)[A-Za-z0-9-]{1,63}(?<!-)\\.)+[A-Za-z]{2,}$",               \
                   "Enter domain: example.com")

    /* URL pattern - supports many protocols */
    #define ARGUS_RE_URL                                                                           \
        MAKE_REGEX("^([a-zA-Z][a-zA-Z0-9+.-]*):\\/\\/[^\\s/$.?#].[^\\s]*$",                        \
                   "Enter URL: https://example.com")

    /* More specific URL patterns */
    #define ARGUS_RE_HTTP                                                                          \
        MAKE_REGEX("^https?:\\/\\/[^\\s/$.?#].[^\\s]*$", "Enter HTTP(S) URL: http://example.com")

    #define ARGUS_RE_FILE_URL                                                                      \
        MAKE_REGEX("^file:\\/\\/[^\\s/$.?#].[^\\s]*$", "Enter file URL: file:///path/to/file")

    /* Basic port number (1-65535) */
    #define ARGUS_RE_PORT                                                                          \
        MAKE_REGEX("^([1-9][0-9]{0,3}|[1-5][0-9]{4}|6[0-4][0-9]{3}|65[0-4][0-9]{2}|655[0-2][0-9]|" \
                   "6553[0-5])$",                                                                  \
                   "Enter port: 1-65535")

    /*
     * Email related patterns
     */

    /* Basic email validation */
    #define ARGUS_RE_EMAIL                                                                         \
        MAKE_REGEX("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", "Enter email: "            \
                                                                        "user@example.com")

    /* More strict email validation */
    #define ARGUS_RE_EMAIL_STRICT                                                                  \
        MAKE_REGEX(                                                                                \
            "^[a-zA-Z0-9.!#$%&'*+/"                                                                \
            "=?^_`{|}~-]+@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61}[a-zA-Z0-9])?(?:\\.[a-zA-Z0-9](?:[a-"    \
            "zA-Z0-9-]{0,61}[a-zA-Z0-9])?)*$",                                                     \
            "Enter RFC-compliant email address")

    /*
     * Date and time patterns
     */

    /* ISO 8601 date (YYYY-MM-DD) */
    #define ARGUS_RE_ISO_DATE                                                                      \
        MAKE_REGEX("^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])$",                  \
                   "Enter date: YYYY-MM-DD")

    /* ISO 8601 datetime (YYYY-MM-DDThh:mm:ss) */
    #define ARGUS_RE_ISOTIME                                                                       \
        MAKE_REGEX(                                                                                \
            "^\\d{4}-([0][1-9]|[1][0-2])-([0][1-9]|[1-2][0-9]|[3][0-1])T([0][0-9]|[1][0-9]|[2]"    \
            "[0-3]):([0-5][0-9]):([0-5][0-9])$",                                                   \
            "Enter datetime: YYYY-MM-DDThh:mm:ss")

    /* MM/DD/YYYY format */
    #define ARGUS_RE_US_DATE                                                                       \
        MAKE_REGEX("^(0[1-9]|1[0-2])\\/(0[1-9]|[12][0-9]|3[01])\\/\\d{4}$", "Enter US date: "      \
                                                                            "MM/DD/YYYY")

    /* DD/MM/YYYY format */
    #define ARGUS_RE_EU_DATE                                                                       \
        MAKE_REGEX("^(0[1-9]|[12][0-9]|3[01])\\/(0[1-9]|1[0-2])\\/\\d{4}$", "Enter EU date: "      \
                                                                            "DD/MM/YYYY")

    /* Time (24h format) */
    #define ARGUS_RE_TIME24                                                                        \
        MAKE_REGEX("^([01]?[0-9]|2[0-3]):[0-5][0-9](:[0-5][0-9])?$", "Enter time: hh:mm[:ss]")

    /*
     * Phone number patterns
     */

    /* International phone number with optional country code */
    #define ARGUS_RE_PHONE_INTL                                                                    \
        MAKE_REGEX("^\\+?[1-9]\\d{1,14}$", "Enter int'l phone: +12345678901")

    /* North American phone number: 123-456-7890 or (123) 456-7890 */
    #define ARGUS_RE_PHONE_US                                                                      \
        MAKE_REGEX("^(\\+?1[-\\s]?)?(\\([0-9]{3}\\)|[0-9]{3})[-\\s]?[0-9]{3}[-\\s]?[0-9]{4}$",     \
                   "Enter US phone: 123-456-7890")

    /* European phone number (general pattern) */
    #define ARGUS_RE_PHONE_EU                                                                      \
        MAKE_REGEX("^\\+?[0-9]{1,3}[-\\s]?[0-9]{2,3}[-\\s]?[0-9]{2,3}[-\\s]?[0-9]{2,3}$",          \
                   "Enter EU phone: +33 123456789")

    /*
     * Identity and security patterns
     */

    /* Username: 3-20 characters, alphanumeric with underscores and hyphens */
    #define ARGUS_RE_USER                                                                          \
        MAKE_REGEX("^[a-zA-Z0-9_-]{3,20}$", "Enter username: 3-20 chars, a-z, 0-9, _-")

    /* Simple password (8+ chars, at least one letter and one number) */
    #define ARGUS_RE_PASSWD                                                                        \
        MAKE_REGEX("^(?=.*[A-Za-z])(?=.*\\d)[A-Za-z\\d]{8,}$",                                     \
                   "Enter password: min 8 chars, letters & numbers")

    /* Strong password (8+ chars with lowercase, uppercase, number, special char) */
    #define ARGUS_RE_PASSWD_STRONG                                                                 \
        MAKE_REGEX("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d)(?=.*[@$!%*?&])[A-Za-z\\d@$!%*?&]{8,}$",       \
                   "Enter secure password: a-z, A-Z, 0-9, symbols")

    /* UUID (version 4) */
    #define ARGUS_RE_UUID                                                                          \
        MAKE_REGEX("^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-4[0-9a-fA-F]{3}-[89abAB][0-9a-fA-F]{3}-[0-9a-"  \
                   "fA-F]{12}$",                                                                   \
                   "Enter UUID v4: xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx")

    /*
     * Geographic and localization patterns
     */

    /* US Zip Code (5 digits, optional 4 digit extension) */
    #define ARGUS_RE_ZIP MAKE_REGEX("^[0-9]{5}(?:-[0-9]{4})?$", "Enter ZIP: 12345 or 12345-6789")

    /* UK Postcode */
    #define ARGUS_RE_UK_POST                                                                       \
        MAKE_REGEX("^[A-Z]{1,2}[0-9][A-Z0-9]? ?[0-9][A-Z]{2}$", "Enter UK postcode: SW1A 1AA")

    /* Canadian Postal Code */
    #define ARGUS_RE_CA_POST                                                                       \
        MAKE_REGEX("^[A-Za-z][0-9][A-Za-z] ?[0-9][A-Za-z][0-9]$", "Enter postal code: A1A 1A1")

    /* Latitude (-90 to 90) with decimal precision */
    #define ARGUS_RE_LATITUDE                                                                      \
        MAKE_REGEX("^[-+]?([1-8]?\\d(\\.\\d+)?|90(\\.0+)?)$", "Enter latitude: -90 to 90")

    /* Longitude (-180 to 180) with decimal precision */
    #define ARGUS_RE_LONGITUDE                                                                     \
        MAKE_REGEX("^[-+]?(180(\\.0+)?|((1[0-7]\\d)|([1-9]?\\d))(\\.\\d+)?)$",                     \
                   "Enter longitude: -180 to 180")

    /*
     * File and path patterns
     */

    /* Unix absolute path */
    #define ARGUS_RE_UNIX_PATH MAKE_REGEX("^(/[^/ ]*)+/?$", "Enter Unix path: /path/to/file")

    /* Windows absolute path (with drive letter) */
    #define ARGUS_RE_WIN_PATH                                                                      \
        MAKE_REGEX("^[a-zA-Z]:\\\\([^\\\\/:*?\"<>|]+(\\\\)?)*$", "Enter Windows path: "            \
                                                                 "C:\\folder\\file")

    /* File name with extension */
    #define ARGUS_RE_FILENAME                                                                      \
        MAKE_REGEX("^[\\w,\\s-]+\\.[A-Za-z]{1,5}$", "Enter filename: name.ext")

    /*
     * Numbers and code patterns
     */

    /* Hexadecimal color (e.g. #FF00FF) */
    #define ARGUS_RE_HEX_COLOR                                                                     \
        MAKE_REGEX("^#?([a-fA-F0-9]{6}|[a-fA-F0-9]{3})$", "Enter hex color: #RRGGBB")

    /* RGB color (e.g. rgb(255,0,255)) */
    #define ARGUS_RE_RGB                                                                           \
        MAKE_REGEX("^rgb\\(\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*,\\s*(\\d{1,3})\\s*\\)$",          \
                   "Enter RGB: rgb(R,G,B)")

    /* Semantic version (major.minor.patch) */
    #define ARGUS_RE_SEMVER                                                                        \
        MAKE_REGEX(                                                                                \
            "^(0|[1-9]\\d*)\\.(0|[1-9]\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|\\d*[a-zA-Z-]["     \
            "0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:\\+([0-9a-"      \
            "zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$",                                                    \
            "Enter semver: X.Y.Z[-pre][+build]")

    /* Positive integer */
    #define ARGUS_RE_POS_INT MAKE_REGEX("^[1-9][0-9]*$", "Enter positive number: 1+")

    /* Negative integer */
    #define ARGUS_RE_NEG_INT MAKE_REGEX("^-[1-9][0-9]*$", "Enter negative number: -1...")

    /* Float (positive or negative with decimal places) */
    #define ARGUS_RE_FLOAT   MAKE_REGEX("^[-+]?[0-9]*\\.?[0-9]+$", "Enter decimal number: 3.14")

    /* Hex number with 0x prefix */
    #define ARGUS_RE_HEX     MAKE_REGEX("^0x[0-9a-fA-F]+$", "Enter hex: 0x1A3F")

#endif /* ARGUS_REGEX */

#endif /* ARGUS_REGEX_H */
