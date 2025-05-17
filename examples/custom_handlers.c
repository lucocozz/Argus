/**
 * Custom handlers example for argus
 * 
 * Demonstrates data transformation with custom handlers:
 * - URL parser that creates a structured URL object
 */

#include "argus/internal/cross_platform.h"
#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * URL processing example - Demonstrates parsing complex data into a 
 * usable structure rather than just validation
 */
typedef struct {
    char *protocol;
    char *host;
    int   port;
    char *path;
    char *query;
} url_t;

// Free handler for URL structures
int url_free_handler(argus_option_t *option)
{
    url_t *url = (url_t*)option->value.as_ptr;

    if (url->protocol) free(url->protocol);
    if (url->host) free(url->host);
    if (url->path) free(url->path);
    if (url->query) free(url->query);

    free(url);

    return ARGUS_SUCCESS;
}

int url_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    // Don't need to check if arg is NULL here, as argus will handle it

    url_t *url = calloc(1, sizeof(url_t));
    if (!url)
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");

    // Assigning the value at the beginning for automatic freeing in case of error
    option->value.as_ptr = url;
    option->is_allocated = true;

    char *protocol_end = strstr(arg, "://");
    if (protocol_end) {
        size_t protocol_len = (size_t)(protocol_end - arg);
        if (!(url->protocol = safe_strndup(arg, protocol_len)))
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
        arg = protocol_end + 3;
    }
    else {
        if (!(url->protocol = safe_strdup("http")))
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    char *host_end = strchr(arg, '/');
    char *port_start = strchr(arg, ':');
    
    size_t host_len;
    if (port_start && (!host_end || port_start < host_end)) {
        host_len = port_start - arg;
        url->port = atoi(port_start + 1);
    } else {
        host_len = host_end ? (size_t)(host_end - arg) : strlen(arg);
        url->port = (strcmp(url->protocol, "https") == 0) ? 443 : 80;
    }
    
    if (!(url->host = safe_strndup(arg, host_len)))
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    
    arg = host_end ? host_end : (arg + strlen(arg));
    
    if (*arg)
    {
        char *query_start = strchr(arg, '?');
        if (query_start) {
            if (!(url->path = safe_strndup(arg, query_start - arg)) ||
                !(url->query = safe_strdup(query_start + 1))) {
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
            }
        }
        else {
            if (!(url->path = safe_strdup(arg)))
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
        }
    }
    else {
        if (!(url->path = safe_strdup("/")))
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }

    return ARGUS_SUCCESS;
}
// You can combine with a custom pre-validator to check if the URL format is valid or
// a custom validator to check if the URL is reachable


// Define helpers macros for URL handlers
#define POSITIONAL_URL(name, ...) \
    POSITIONAL_BASE(name, VALUE_TYPE_CUSTOM, HANDLER(url_handler), FREE_HANDLER(url_free_handler), __VA_ARGS__)

#define OPTION_URL(short_name, long_name, ...) \
    OPTION_BASE(short_name, long_name, VALUE_TYPE_CUSTOM, HANDLER(url_handler), FREE_HANDLER(url_free_handler), __VA_ARGS__)


// Define the options
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    OPTION_URL('p', "proxy", HELP("Proxy URL to connect to")),
    POSITIONAL_URL("target", HELP("Target URL to connect to"))
)


void print_url(url_t *url)
{
    printf("Protocol: %s\n", url->protocol);
    printf("Host: %s\n", url->host);
    printf("Port: %d\n", url->port);
    printf("Path: %s\n", url->path);
    printf("Query: %s\n", url->query ? url->query : "(none)");
}


int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "custom_handlers_example", "1.0.0");
    argus.description = "Example of data transformation with custom handlers";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;
    
    url_t *target = argus_get(argus, "target").as_ptr;

    printf("Target URL:\n");
    print_url(target);
    printf("\n");

    if (argus_is_set(argus, "proxy"))
    {
        url_t *proxy = argus_get(argus, "proxy").as_ptr;
        printf("Proxy URL:\n");
        print_url(proxy);
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
