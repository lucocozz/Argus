/**
 * Multi-values example for argus
 * 
 * Demonstrates different approaches to working with array and map data structures:
 * 1. Direct access (original approach)
 * 2. Element access helpers
 * 3. Iterator approach
 * 
 * Examples:
 * - Arrays:
 *   ./multi_values --names="john,alice,bob" --ids="1,2,3-5,10"
 * 
 * - Maps (different types):
 *   ./multi_values --env="name=value,host=localhost" 
 *   ./multi_values --ports="http=80,https=443,ftp=21"
 *   ./multi_values --scales="width=0.5,height=1.5,zoom=2.0"
 *   ./multi_values --flags="debug=true,verbose=yes,logging=on,cache=false"
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Define options
ARGUS_OPTIONS(
    options,
    // Standard options
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    VERSION_OPTION(FLAGS(FLAG_EXIT)),
    
    // Demo options to select processing approach
    OPTION_INT('a', "approach", HELP("Processing approach: 1=direct, 2=element, 3=iterator"), 
               DEFAULT(1), RANGE(1, 3)),
    
    // Array options with custom HINT()
    OPTION_ARRAY_STRING('n', "names", HELP("Array of user names (comma-separated)"),
                       HINT("NAME"),
                       FLAGS(FLAG_SORTED)),
                       
    OPTION_ARRAY_INT('i', "ids", HELP("Array of ID numbers or ranges (e.g., 1,2,3-5)"),
                    HINT("ID"),
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    
    // Map options with custom HINT()
    OPTION_MAP_STRING('\0', "env", HELP("Environment variables (key=value pairs)"),
                    HINT("VALUE"),
                    FLAGS(FLAG_SORTED_KEY)),
                     
    OPTION_MAP_INT('\0', "ports", HELP("Port numbers for services"),
                    HINT("PORT"),
                    FLAGS(FLAG_UNIQUE_VALUE)),
                  
    OPTION_MAP_FLOAT('\0', "scales", HELP("Scaling factors for dimensions"),
                    HINT("FACTOR"),
                    FLAGS(FLAG_SORTED_VALUE)),
                    
    OPTION_MAP_BOOL('\0', "flags", HELP("Feature flags configuration"),
                    HINT("FLAG"))
)

// Process arrays using direct approach (original)
void process_arrays_direct(argus_t argus)
{
    printf("=== APPROACH 1: DIRECT ACCESS ===\n\n");
    
    // Handle array of strings
    if (argus_is_set(argus, "names"))
    {
        argus_value_t *names = argus_get(argus, "names").as_array;
        size_t count = argus_count(argus, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: \"%s\"\n", i, names[i].as_string);
        printf("\n");
    }

    // Handle array of integers
    if (argus_is_set(argus, "ids"))
    {
        argus_value_t *ids = argus_get(argus, "ids").as_array;
        size_t count = argus_count(argus, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: %d\n", i, ids[i].as_int);
        printf("\n");
    }
}

// Process maps using direct approach (original)
void process_maps_direct(argus_t argus)
{
    // Handle map of strings
    if (argus_is_set(argus, "env"))
    {
        argus_pair_t *env = argus_get(argus, "env").as_map;
        size_t count = argus_count(argus, "env");
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => '%s'\n", 
                   env[i].key, 
                   env[i].value.as_string);
        }
        printf("\n");
    }

    // Handle map of integers
    if (argus_is_set(argus, "ports"))
    {
        argus_pair_t *ports = argus_get(argus, "ports").as_map;
        size_t count = argus_count(argus, "ports");
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %d\n", 
                   ports[i].key, 
                   ports[i].value.as_int);
        }
        printf("\n");
    }

    // Handle map of floats
    if (argus_is_set(argus, "scales"))
    {
        argus_pair_t *scales = argus_get(argus, "scales").as_map;
        size_t count = argus_count(argus, "scales");
        
        printf("Scaling factors (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %.3f\n", 
                   scales[i].key, 
                   scales[i].value.as_float);
        }
        printf("\n");
    }

    // Handle map of booleans
    if (argus_is_set(argus, "flags"))
    {
        argus_pair_t *flags = argus_get(argus, "flags").as_map;
        size_t count = argus_count(argus, "flags");
        
        printf("Feature flags (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            printf("  '%s' => %s\n", 
                   flags[i].key, 
                   flags[i].value.as_bool ? "enabled" : "disabled");
        }
        printf("\n");
    }
}

// Process arrays using element access helpers
void process_arrays_element(argus_t argus)
{
    printf("=== APPROACH 2: ELEMENT ACCESS HELPERS ===\n\n");
    
    // Handle array of strings
    if (argus_is_set(argus, "names")) {
        size_t count = argus_count(argus, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char* name = argus_array_get(argus, "names", i).as_string;
            printf("  [%zu]: \"%s\"\n", i, name);
        }
        printf("\n");
    }

    // Handle array of integers
    if (argus_is_set(argus, "ids"))
    {
        size_t count = argus_count(argus, "ids");
        
        printf("ID numbers array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            int id = argus_array_get(argus, "ids", i).as_int;
            printf("  [%zu]: %d\n", i, id);
        }
        printf("\n");
    }
}

// Process maps using element access helpers
void process_maps_element(argus_t argus)
{
    // First show direct key access examples

    // Handle map of strings
    if (argus_is_set(argus, "env"))
    {
        printf("Direct key lookups:\n");
        
        const char* user = argus_map_get(argus, "env", "USER").as_string;
        if (user)
            printf("  USER = %s\n", user);

        const char* home = argus_map_get(argus, "env", "HOME").as_string;
        if (home)
            printf("  HOME = %s\n", home);
        printf("\n");
    }

    // Handle map of integers with direct key access
    if (argus_is_set(argus, "ports"))
    {
        printf("Common port lookups:\n");
        
        int http = argus_map_get(argus, "ports", "http").as_int;
        if (http)
            printf("  HTTP port: %d\n", http);

        int https = argus_map_get(argus, "ports", "https").as_int;
        if (https)
            printf("  HTTPS port: %d\n", https);
        printf("\n");
    }

    // Now show complete listings using regular iteration with map value helper

    // Handle map of strings
    if (argus_is_set(argus, "env"))
    {
        size_t count = argus_count(argus, "env");
        argus_pair_t *env = argus_get(argus, "env").as_map;
        
        printf("Environment variables (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = env[i].key;
            const char *value = argus_map_get(argus, "env", key).as_string;
            printf("  '%s' => '%s'\n", key, value);
        }
        printf("\n");
    }

    // Handle map of integers (similarly for other maps)
    if (argus_is_set(argus, "ports"))
    {
        size_t count = argus_count(argus, "ports");
        argus_pair_t *ports = argus_get(argus, "ports").as_map;
        
        printf("Port mappings (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *key = ports[i].key;
            int value = argus_map_get(argus, "ports", key).as_int;
            printf("  '%s' => %d\n", key, value);
        }
        printf("\n");
    }
}

// Process arrays using iterator approach
void process_arrays_iterator(argus_t argus) {
    printf("=== APPROACH 3: ITERATORS ===\n\n");
    
    // Handle array of strings
    if (argus_is_set(argus, "names"))
    {
        argus_array_it_t it = argus_array_it(argus, "names");

        printf("Names array (%zu items):\n", it._count);
        for (int i = 0; argus_array_next(&it); i++)
            printf("  [%d]: \"%s\"\n", i++, it.value.as_string);
        printf("\n");
    }

    // Handle array of integers
    if (argus_is_set(argus, "ids"))
    {
        argus_array_it_t it = argus_array_it(argus, "ids");

        printf("ID numbers array (%zu items):\n", it._count);
        for (int i = 0; argus_array_next(&it); i++)
            printf("  [%d]: %d\n", i++, it.value.as_int);
        printf("\n");
    }
}

// Process maps using iterator approach
void process_maps_iterator(argus_t argus)
{
    // Handle map of strings
    if (argus_is_set(argus, "env"))
    {
        argus_map_it_t it = argus_map_it(argus, "env");

        printf("Environment variables (%zu items):\n", it._count);
        while (argus_map_next(&it))
            printf("  '%s' => '%s'\n", it.key, it.value.as_string);
        printf("\n");
    }

    // Handle map of integers
    if (argus_is_set(argus, "ports"))
    {
        argus_map_it_t it = argus_map_it(argus, "ports");

        printf("Port mappings (%zu items):\n", it._count);
        while (argus_map_next(&it))
            printf("  '%s' => %d\n", it.key, it.value.as_int);
        printf("\n");
    }

    // Handle map of floats
    if (argus_is_set(argus, "scales"))
    {
        argus_map_it_t it = argus_map_it(argus, "scales");

        printf("Scaling factors (%zu items):\n", it._count);
        while (argus_map_next(&it))
            printf("  '%s' => %.3f\n", it.key, it.value.as_float);
        printf("\n");
    }

    // Handle map of booleans
    if (argus_is_set(argus, "flags"))
    {
        argus_map_it_t it = argus_map_it(argus, "flags");

        printf("Feature flags (%zu items):\n", it._count);
        while (argus_map_next(&it))
            printf("  '%s' => %s\n", it.key, it.value.as_bool ? "enabled" : "disabled");
        printf("\n");
    }

    // Example of iterator re-use for multiple scans
    if (argus_is_set(argus, "flags"))
    {
        argus_map_it_t it = argus_map_it(argus, "flags");

        printf("Enabled features only:\n");
        while (argus_map_next(&it))
            if (it.value.as_bool)
                printf("  '%s'\n", it.key);

        printf("\nDisabled features only:\n");
        argus_map_reset(&it);  // Reset the iterator for second pass
        while (argus_map_next(&it))
            if (!it.value.as_bool)
                printf("  '%s'\n", it.key);
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    // Initialize argus
    argus_t argus = argus_init(options, "multi_values", "1.0.0");
    argus.description = "Example of multi-value options with different access approaches";

    // Parse command-line arguments
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Get the selected approach
    int approach = argus_get(argus, "approach").as_int;
    
    // Process data using the selected approach
    switch (approach) {
        case 1:
            process_arrays_direct(argus);
            process_maps_direct(argus);
            break;
        case 2:
            process_arrays_element(argus);
            process_maps_element(argus);
            break;
        case 3:
            process_arrays_iterator(argus);
            process_maps_iterator(argus);
            break;
        default:
            printf("Invalid approach selected. Please use 1, 2, or 3.\n");
            break;
    }

    // Free resources
    argus_free(&argus);
    return 0;
}
