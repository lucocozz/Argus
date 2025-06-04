/**
 * Validators example for argus
 * 
 * Demonstrates built-in validators and custom validators with data parameters
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Cross-platform IP validation functions
static int parse_ipv4(const char *ip_str, uint32_t *ip_out)
{
    unsigned int parts[4];
    int count = sscanf(ip_str, "%u.%u.%u.%u", &parts[0], &parts[1], &parts[2], &parts[3]);
    
    if (count != 4)
        return 0;
    
    for (int i = 0; i < 4; i++) {
        if (parts[i] > 255)
            return 0;
    }
    
    *ip_out = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    return 1;
}

static int is_valid_ipv6(const char *ip_str)
{
    int colon_count = 0;
    int double_colon = 0;
    int hex_groups = 0;
    const char *p = ip_str;
    
    while (*p)
    {
        if (*p == ':')
        {
            colon_count++;
            if (*(p + 1) == ':') {
                if (double_colon)
                    return 0; // Multiple double colons not allowed
                double_colon = 1;
                p++;
            }
        }
        else if (isxdigit(*p))
        {
            int hex_count = 0;
            while (isxdigit(*p) && hex_count < 4) {
                p++;
                hex_count++;
            }
            if (hex_count == 0 || hex_count > 4)
                return 0;
            hex_groups++;
            continue;
        }
        else
            return 0; // Invalid character
        p++;
    }
    
    // Basic validation: should have appropriate number of groups
    return (double_colon || hex_groups == 8) && colon_count <= 7;
}

int int_divisible_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    int divisor = (int)data.custom;
    int value = option->value.as_int;
    
    if (value % divisor != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Value must be divisible by %d", divisor);
    }
    
    return ARGUS_SUCCESS;
}

int domain_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *domain = (const char*)data.custom;
    const char *email = option->value.as_string;

    if (domain == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INTERNAL, 
                          "Internal error: Domain validator requires a domain");
    }

    const char *at = strchr(email, '@');
    if (!at) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email must contain '@' symbol");
    }
    
    if (strcmp(at + 1, domain) != 0) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                          "Email must use the domain '%s'", domain);
    }
    
    return ARGUS_SUCCESS;
}

typedef struct {
    bool allow_ipv6;
    bool allow_private;
} ip_validator_config_t;

int ip_address_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
    argus_option_t *option = (argus_option_t *)option_ptr;
    const char *ip_str = option->value.as_string;
    ip_validator_config_t *config = (ip_validator_config_t *)data.custom;
    
    uint32_t ipv4;
    if (parse_ipv4(ip_str, &ipv4))
    {
        if (!config->allow_private)
        {
            // 10.0.0.0/8
            if ((ipv4 & 0xFF000000) == 0x0A000000) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                  "Private IP addresses are not allowed (10.0.0.0/8)");
                return ARGUS_ERROR_INVALID_VALUE;
            }
            // 172.16.0.0/12
            if ((ipv4 & 0xFFF00000) == 0xAC100000) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                  "Private IP addresses are not allowed (172.16.0.0/12)");
                return ARGUS_ERROR_INVALID_VALUE;
            }
            // 192.168.0.0/16
            if ((ipv4 & 0xFFFF0000) == 0xC0A80000) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                                  "Private IP addresses are not allowed (192.168.0.0/16)");
                return ARGUS_ERROR_INVALID_VALUE;
            }
        }
        
        return ARGUS_SUCCESS;
    }

    if (config->allow_ipv6) {
        if (is_valid_ipv6(ip_str))
            return ARGUS_SUCCESS;
    }
    
    ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_VALUE,
                      config->allow_ipv6 ? 
                      "Invalid IP address format (IPv4 or IPv6)" : 
                      "Invalid IPv4 address format");
    
    return ARGUS_ERROR_INVALID_VALUE;
}

#define V_DOMAINE(_domain_) \
    MAKE_VALIDATOR(domain_validator, _V_DATA_CUSTOM_(_domain_), ORDER_POST)
#define V_DIVISIBLE_BY(_divisor_) \
    MAKE_VALIDATOR(int_divisible_validator, _V_DATA_CUSTOM_(_divisor_), ORDER_POST)
#define V_IP_ADDRESS(allow_ipv6, allow_private) \
    MAKE_VALIDATOR(ip_address_validator, \
        _V_DATA_CUSTOM_(&((ip_validator_config_t){allow_ipv6, allow_private})), \
        ORDER_POST)

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    // Built-in choices validator
    OPTION_STRING('l', "log-level", HELP("Log level"), 
                DEFAULT("info"), 
                VALIDATOR(V_CHOICES_STRING("debug", "info", "warning", "error"))),

    // Custom validator using custom data parameter (int)
    OPTION_INT('n', "number", HELP("Number (must be divisible by 5)"), 
               VALIDATOR(V_DIVISIBLE_BY(5), V_RANGE(1, 100)),
               DEFAULT(10)),

    // Custom validator using custom data parameter (string)
    OPTION_STRING('e', "email", HELP("Email address (company domain)"),
                VALIDATOR(V_DOMAINE("example.com"), V_LENGTH(5, 30))),

    // Custom validator using custom data parameter (struct)
    OPTION_STRING('i', "ip-address", 
                HELP("Server IP address (IPv4 only, no private addresses)"),
                VALIDATOR(V_IP_ADDRESS(false, false))),   
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "validators_example", "1.0.0");
    argus.description = "Example of validators with custom data parameters";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    if (argus_is_set(argus, "log-level"))
        printf("Log level: %s\n", argus_get(argus, "log-level").as_string);
    if (argus_is_set(argus, "ip-address"))
        printf("IP address: %s\n", argus_get(argus, "ip-address").as_string);
    if (argus_is_set(argus, "email"))
        printf("Email: %s\n", argus_get(argus, "email").as_string);
    if (argus_is_set(argus, "number"))
        printf("Number: %d\n", argus_get(argus, "number").as_int);

    argus_free(&argus);
    return 0;
}
