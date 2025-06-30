/**
 * Nested commands example for argus
 * 
 * Demonstrates:
 * - Nested subcommands similar to complex CLI tools
 * - Path formats for accessing values (relative, absolute, root-level)
 */

#include "argus.h"
#include <stdio.h>
#include <stdlib.h>

// Action handlers
int service_create_action(argus_t *argus, void *data);
int service_list_action(argus_t *argus, void *data);
int config_set_action(argus_t *argus, void *data);
int config_get_action(argus_t *argus, void *data);

// Define options for "service create" command
ARGUS_OPTIONS(
    service_create_options,
    HELP_OPTION(),
    OPTION_STRING('n', "name", HELP("Service name"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('i', "image", HELP("Container image"), FLAGS(FLAG_REQUIRED)),
)

// Define options for "service list" command
ARGUS_OPTIONS(
    service_list_options,
    HELP_OPTION(),
    OPTION_FLAG('a', "all", HELP("Show all services, including stopped ones")),
)

// Define options for the "service" parent command
ARGUS_OPTIONS(
    service_options,
    HELP_OPTION(),
    
    SUBCOMMAND("create", service_create_options, 
               HELP("Create a new service"), 
               ACTION(service_create_action)),
    SUBCOMMAND("list", service_list_options, 
               HELP("List services"), 
               ACTION(service_list_action)),
)

// Define options for "config set" command
ARGUS_OPTIONS(
    config_set_options,
    HELP_OPTION(),
    POSITIONAL_STRING("key", HELP("Configuration key")),
    POSITIONAL_STRING("value", HELP("Configuration value")),
)

// Define options for "config get" command
ARGUS_OPTIONS(
    config_get_options,
    HELP_OPTION(),
    POSITIONAL_STRING("key", HELP("Configuration key")),
)

// Define options for the "config" parent command
ARGUS_OPTIONS(
    config_options,
    HELP_OPTION(),
    
    SUBCOMMAND("set", config_set_options, 
               HELP("Set a configuration value"), 
               ACTION(config_set_action)),
    SUBCOMMAND("get", config_get_options, 
               HELP("Get a configuration value"), 
               ACTION(config_get_action)),
)

// Define main options with top-level subcommands
ARGUS_OPTIONS(
    options,
    HELP_OPTION(),
    VERSION_OPTION(),
    
    // Global option at root level
    OPTION_FLAG('d', "debug", HELP("Enable debug mode")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.log")),
    
    SUBCOMMAND("service", service_options, 
               HELP("Service management commands")),
    SUBCOMMAND("config", config_options, 
               HELP("Configuration commands")),
)

int main(int argc, char **argv)
{
    argus_t argus = argus_init(options, "nested_commands", "1.0.0");
    argus.description = "Example of nested subcommands and path formats";
    
    int status = argus_parse(&argus, argc, argv);
    if (status != ARGUS_SUCCESS)
        return status;

    // Root-level options can be accessed directly from main
    bool debug = argus_get(&argus, ".debug").as_bool;
    if (debug)
        printf("[Debug mode enabled at root level]\n");

    if (argus_has_command(&argus))
        status = argus_exec(&argus, NULL);
    else
        printf("No command specified. Use --help to see available commands.\n");

    argus_free(&argus);
    return 0;
}

// Command action implementations
int service_create_action(argus_t *argus, void *data)
{
    (void)data;
    
    // Different ways to access option values
    
    // 1. Relative path (relative to current subcommand)
    // - When inside "service create" handler, "name" refers to "service.create.name"
    const char *name = argus_get(argus, "name").as_string;
    const char *image = argus_get(argus, "image").as_string;
    
    // 2. Absolute path (full path from root)
    // - Explicitly specifies the full path
    const char *name_abs = argus_get(argus, "service.create.name").as_string;
    (void)name_abs;
    
    // 3. Root-level path (access options at root level)
    // - Starts with "." to force root level
    const char *output = argus_get(argus, ".output").as_string;
    bool debug = argus_get(argus, ".debug").as_bool;
    
    printf("Creating service '%s' using image '%s'\n", name, image);
    printf("Output file: %s\n", output);
    if (debug) printf("Debug mode enabled\n");
    
    // Check if commands or options are set using argus_is_set()
    printf("\nCommand check:\n");
    printf("- 'service' command is set: %s\n", 
           argus_is_set(argus, "service") ? "yes" : "no");
    printf("- 'service.create' command is set: %s\n", 
           argus_is_set(argus, "service.create") ? "yes" : "no");
    
    return 0;
}

int service_list_action(argus_t *argus, void *data)
{
    (void)data;
    
    // Relative path (within current subcommand context)
    bool all = argus_get(argus, "all").as_bool;
    
    // Root-level option
    bool debug = argus_get(argus, ".debug").as_bool;
    
    printf("Listing services (all=%s)\n", all ? "true" : "false");
    if (debug) printf("Debug mode enabled\n");
    
    // Demonstrating argus_is_set with options
    printf("\nOption check:\n");
    printf("- 'all' option is set: %s\n", 
           argus_is_set(argus, "all") ? "yes" : "no");
    printf("- Root-level 'debug' option is set: %s\n", 
           argus_is_set(argus, ".debug") ? "yes" : "no");
    
    return 0;
}

int config_set_action(argus_t *argus, void *data)
{
    (void)data;
    
    // Access positional arguments (relative path)
    const char *key = argus_get(argus, "key").as_string;
    const char *value = argus_get(argus, "value").as_string;
    
    // Alternative: absolute path
    const char *key_abs = argus_get(argus, "config.set.key").as_string;
    (void)key_abs;
    
    // Check if positional arguments are set
    printf("Setting config '%s' to '%s'\n", key, value);
    printf("\nPositional check:\n");
    printf("- 'key' positional is set: %s\n", 
           argus_is_set(argus, "key") ? "yes" : "no");
    printf("- 'value' positional is set: %s\n", 
           argus_is_set(argus, "value") ? "yes" : "no");

    return 0;
}

int config_get_action(argus_t *argus, void *data)
{
    (void)data;
    const char *key = argus_get(argus, "config.get.key").as_string;
    
    printf("Getting config value for '%s'\n", key);
    return 0;
}


