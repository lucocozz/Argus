/**
 * Custom handlers example for argus
 * 
 * Demonstrates data transformation with custom handlers:
 * - URL parser that creates a structured URL object
 * - Color parser that converts various formats into RGB values
 * - Coordinate parser that handles different coordinate formats
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 
 * URL processing example - Demonstrates parsing complex data into a 
 * usable structure rather than just validation
 */
typedef struct {
    char* protocol;
    char* host;
    int port;
    char* path;
    char* query;
} url_t;

/**
 * Helper function to free a url_t structure safely
 */
void free_url(url_t *url)
{
    if (!url) return;
    if (url->protocol) free(url->protocol);
    if (url->host) free(url->host);
    if (url->path) free(url->path);
    if (url->query) free(url->query);
    free(url);
}

int url_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "URL is required");
    }
    
    // Allocate and initialize the URL structure
    url_t *url = calloc(1, sizeof(url_t));  // Use calloc instead of malloc + manual init
    if (!url) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Parse protocol
    char *protocol_end = strstr(arg, "://");
    if (protocol_end) {
        size_t protocol_len = protocol_end - arg;
        if (!(url->protocol = strndup(arg, protocol_len))) {  // Use strndup for cleaner code
            free_url(url);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
        }
        arg = protocol_end + 3;  // Move past the protocol part
    } else {
        // Default protocol
        if (!(url->protocol = strdup("http"))) {
            free_url(url);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
        }
    }
    
    // Find end of host and potential port
    char *host_end = strchr(arg, '/');
    char *port_start = strchr(arg, ':');
    
    // Extract host
    size_t host_len;
    if (port_start && (!host_end || port_start < host_end)) {
        host_len = port_start - arg;
        url->port = atoi(port_start + 1);  // Extract port
    } else {
        host_len = host_end ? (size_t)(host_end - arg) : strlen(arg);
        // Default port based on protocol
        url->port = (strcmp(url->protocol, "https") == 0) ? 443 : 80;
    }
    
    // Allocate and copy host
    if (!(url->host = strndup(arg, host_len))) {
        free_url(url);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Update position for path parsing
    arg = host_end ? host_end : (arg + strlen(arg));
    
    // Extract path and query
    if (*arg) {
        char *query_start = strchr(arg, '?');
        if (query_start) {
            // Path exists with query
            if (!(url->path = strndup(arg, query_start - arg)) ||
                !(url->query = strdup(query_start + 1))) {
                free_url(url);
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
            }
        } else {
            // Path without query
            if (!(url->path = strdup(arg))) {
                free_url(url);
                ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
            }
        }
    } else {
        // No path
        if (!(url->path = strdup("/"))) {
            free_url(url);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
        }
    }
    
    // Store the URL structure
    option->value.as_ptr = url;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

// Free handler for URL structures
int url_free_handler(argus_option_t *option)
{
    url_t *url = (url_t*)option->value.as_ptr;
    free_url(url);
    return ARGUS_SUCCESS;
}

/*
 * Color processing example - Demonstrates converting different formats 
 * into a uniform RGB structure
 */
typedef struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} rgb_color_t;

int color_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Color value is required");
    }
    
    // Allocate the color structure
    rgb_color_t *color = calloc(1, sizeof(rgb_color_t));  // Use calloc for simpler initialization
    if (!color) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Handle hex format (#RRGGBB or #RGB)
    if (arg[0] == '#') {
        unsigned int r, g, b;
        size_t len = strlen(arg + 1);
        
        if (len == 6 && sscanf(arg, "#%02x%02x%02x", &r, &g, &b) == 3) {
            // #RRGGBB format
            color->r = (unsigned char)r;
            color->g = (unsigned char)g;
            color->b = (unsigned char)b;
        } 
        else if (len == 3 && sscanf(arg, "#%1x%1x%1x", &r, &g, &b) == 3) {
            // #RGB format - expand to full values (e.g., 5 -> 55)
            color->r = (unsigned char)((r << 4) | r);
            color->g = (unsigned char)((g << 4) | g);
            color->b = (unsigned char)((b << 4) | b);
        }
        else {
            free(color);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                            "Invalid hex color format: %s (expected #RRGGBB or #RGB)", arg);
        }
    }
    // rgb(r,g,b) format
    else if (strncmp(arg, "rgb(", 4) == 0 && arg[strlen(arg)-1] == ')') {
        int r, g, b;
        
        // Parse directly with sscanf
        if (sscanf(arg, "rgb(%d,%d,%d)", &r, &g, &b) == 3) {
            color->r = (unsigned char)r;
            color->g = (unsigned char)g;
            color->b = (unsigned char)b;
        } else {
            free(color);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                            "Invalid RGB format: %s (expected rgb(r,g,b))", arg);
        }
    }
    // Named colors - use a structure for cleaner mapping
    else {
        struct { const char *name; unsigned char r, g, b; } named_colors[] = {
            {"red",    255, 0,   0},
            {"green",  0,   255, 0},
            {"blue",   0,   0,   255},
            {"black",  0,   0,   0},
            {"white",  255, 255, 255},
            {NULL, 0, 0, 0}  // End marker
        };
        
        int i;
        for (i = 0; named_colors[i].name; i++) {
            if (strcmp(arg, named_colors[i].name) == 0) {
                color->r = named_colors[i].r;
                color->g = named_colors[i].g;
                color->b = named_colors[i].b;
                break;
            }
        }
        
        if (!named_colors[i].name) {
            free(color);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                            "Unrecognized color format: %s", arg);
        }
    }
    
    option->value.as_ptr = color;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

// Free handler for color structures
int color_free_handler(argus_option_t *option)
{
    rgb_color_t *color = (rgb_color_t*)option->value.as_ptr;
    if (color) {
        free(color);
    }
    return ARGUS_SUCCESS;
}

/*
 * Coordinate processing example - Demonstrates parsing different 
 * coordinate formats into a standardized structure
 */
typedef struct {
    double lat;
    double lon;
    enum { DEG_MIN_SEC, DECIMAL, UNKNOWN } format;
} geo_coord_t;

int coordinate_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    if (arg == NULL) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MISSING_VALUE, "Coordinate is required");
    }
    
    // Allocate and initialize the coordinate structure
    geo_coord_t *coord = calloc(1, sizeof(geo_coord_t));
    if (!coord) {
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_MEMORY, "Memory allocation failed");
    }
    
    // Check for decimal format: "lat,lon"
    if (strchr(arg, ',')) {
        double lat, lon;
        if (sscanf(arg, "%lf,%lf", &lat, &lon) == 2) {
            coord->lat = lat;
            coord->lon = lon;
            coord->format = DECIMAL;
        } else {
            free(coord);
            ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                            "Invalid coordinate format: %s (expected lat,lon)", arg);
        }
    }
    // Check for degrees-minutes-seconds format
    else if (strchr(arg, '*') && (strchr(arg, 'N') || strchr(arg, 'S')) && 
            (strchr(arg, 'E') || strchr(arg, 'W'))) {
        // In a real implementation, this would parse DMS format
        // For example: 40*45'30"N,74*0'23"W
        
        // Dummy implementation for the example
        coord->lat = 40.7128;  // New York
        coord->lon = -74.0060;
        coord->format = DEG_MIN_SEC;
    }
    else {
        free(coord);
        ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_FORMAT,
                        "Unrecognized coordinate format: %s", arg);
    }
    
    option->value.as_ptr = coord;
    option->is_allocated = true;
    return ARGUS_SUCCESS;
}

// Free handler for coordinate structures
int coordinate_free_handler(argus_option_t *option)
{
    geo_coord_t *coord = (geo_coord_t*)option->value.as_ptr;
    if (coord) {
        free(coord);
    }
    return ARGUS_SUCCESS;
}

// Define options with custom handlers
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // URL parsing with custom structure
    OPTION_BASE('u', "url", 
                VALUE_TYPE_CUSTOM,
                HELP("Target URL (transforms into a structured URL object)"),
                HANDLER(url_handler),
                FREE_HANDLER(url_free_handler)),
    
    // Color parsing with different formats
    OPTION_BASE('c', "color", 
                VALUE_TYPE_CUSTOM,
                HELP("Color in hex (#RRGGBB), rgb(r,g,b) or named format"),
                HANDLER(color_handler),
                FREE_HANDLER(color_free_handler)),
    
    // Coordinate parsing
    OPTION_BASE('l', "location", 
                VALUE_TYPE_CUSTOM,
                HELP("Geographic coordinates in decimal or DMS format"),
                HANDLER(coordinate_handler),
                FREE_HANDLER(coordinate_free_handler))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "custom_handlers_example", "1.0.0");
    argus.description = "Example of data transformation with custom handlers";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS) {
        return status;
    }
    
    // Process URL if provided
    if (argus_is_set(argus, "url")) {
        url_t *url = argus_get(argus, "url").as_ptr;
        
        printf("URL information:\n");
        printf("  Protocol: %s\n", url->protocol);
        printf("  Host: %s\n", url->host);
        printf("  Port: %d\n", url->port);
        printf("  Path: %s\n", url->path);
        printf("  Query: %s\n", url->query ? url->query : "(none)");
        printf("\n");
    }
    
    // Process color if provided
    if (argus_is_set(argus, "color")) {
        rgb_color_t *color = argus_get(argus, "color").as_ptr;
        
        printf("Color components:\n");
        printf("  Red: %d\n", color->r);
        printf("  Green: %d\n", color->g);
        printf("  Blue: %d\n", color->b);
        printf("  Hex: #%02X%02X%02X\n", color->r, color->g, color->b);
        printf("\n");
    }
    
    // Process coordinates if provided
    if (argus_is_set(argus, "location")) {
        geo_coord_t *coord = argus_get(argus, "location").as_ptr;
        
        printf("Geographic coordinates:\n");
        printf("  Latitude: %.6f*\n", coord->lat);
        printf("  Longitude: %.6f*\n", coord->lon);
        printf("  Format: %s\n", 
               coord->format == DECIMAL ? "Decimal Degrees" : 
               coord->format == DEG_MIN_SEC ? "Degrees-Minutes-Seconds" : 
               "Unknown");
        printf("\n");
    }
    
    argus_free(&argus);
    return 0;
}
