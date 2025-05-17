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

ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),

    OPTION_ARRAY_STRING('n', "names", HELP("Array of names (e.g., john,alice,bob)")),
    OPTION_ARRAY_STRING('w', "words", HELP("Sorted array of words"),
                       FLAGS(FLAG_SORTED)),
    OPTION_ARRAY_INT('i', "ids", HELP("Array of ID numbers or ranges (e.g., 1,2,3-5)"),
                    FLAGS(FLAG_UNIQUE | FLAG_SORTED)),
    OPTION_MAP_STRING('\0', "env", HELP("Environment variables (key=value pairs)"),
                    FLAGS(FLAG_SORTED_KEY)),
    OPTION_MAP_INT('\0', "ports", HELP("Port numbers for services"),
                    FLAGS(FLAG_UNIQUE_VALUE)),
    OPTION_MAP_FLOAT('\0', "scales", HELP("Scaling factors for dimensions"),
                    FLAGS(FLAG_SORTED_VALUE))
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "multi_values", "1.0.0");
    argus.description = "Example of multi-value";

    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Print array values using direct access
    if (argus_is_set(argus, "words"))
    {
        argus_value_t *words = argus_get(argus, "words").as_array;
        size_t count = argus_count(argus, "words");
        
        printf("Words array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i)
            printf("  [%zu]: \"%s\"\n", i, words[i].as_string);
        printf("\n");
    }

    // Print array values using element access helpers
    if (argus_is_set(argus, "names")) {
        size_t count = argus_count(argus, "names");
        
        printf("Names array (%zu items):\n", count);
        for (size_t i = 0; i < count; ++i) {
            const char *name = argus_array_get(argus, "names", i).as_string;
            printf("  [%zu]: \"%s\"\n", i, name);
        }
        printf("\n");
    }


    // Print array values using iterators
    if (argus_is_set(argus, "ids"))
    {
        argus_array_it_t it = argus_array_it(argus, "ids");

        printf("ID numbers array (%zu items):\n", it._count);
        for (int i = 0; argus_array_next(&it); i++)
            printf("  [%d]: %d\n", i++, it.value.as_int);
        printf("\n");
    }

    // Print map values using direct access
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

    // Print map values using element access helpers
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

    // Print map values using iterator approach
    if (argus_is_set(argus, "scales"))
    {
        argus_map_it_t it = argus_map_it(argus, "scales");

        printf("Scaling factors (%zu items):\n", it._count);
        while (argus_map_next(&it))
            printf("  '%s' => %.3f\n", it.key, it.value.as_float);
        printf("\n");
    }

    argus_free(&argus);
    return 0;
}
