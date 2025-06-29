#include <criterion/criterion.h>
#include "argus.h"
#include "argus/internal/parsing.h"
#include "argus/internal/utils.h"
#include <stdlib.h>
#include <string.h>

// Test options
ARGUS_OPTIONS(
    env_options,
    HELP_OPTION(),
    OPTION_STRING('H', "host", HELP("Server hostname"), ENV_VAR("HOST")),
    OPTION_INT('p', "port", HELP("Server port"), FLAGS(FLAG_AUTO_ENV)),
    OPTION_STRING('d', "database", HELP("Database connection string"),
                ENV_VAR("DATABASE_URL"), FLAGS(FLAG_NO_ENV_PREFIX)),
    OPTION_INT('t', "timeout", HELP("Connection timeout in seconds"),
                DEFAULT(30), ENV_VAR("FORCE_TIMEOUT"), FLAGS(FLAG_ENV_OVERRIDE)),
)

// Helper function to setup and cleanup environment
void setup_env(void)
{
    setenv("TEST_HOST", "env-server.example.com", 1);
    setenv("TEST_PORT", "9000", 1);
    setenv("DATABASE_URL", "postgres://user:pass@localhost/db", 1);
    setenv("TEST_FORCE_TIMEOUT", "60", 1);
}

void teardown_env(void)
{
    unsetenv("TEST_HOST");
    unsetenv("TEST_PORT");
    unsetenv("DATABASE_URL");
    unsetenv("TEST_FORCE_TIMEOUT");
}

// Test load_env_vars function
Test(env_vars, load_env_vars, .init = setup_env, .fini = teardown_env)
{
    argus_t argus = argus_init(env_options, "test_program", "1.0.0");
    argus.env_prefix = "TEST";
    
    // Options should not be set initially
    argus_option_t *host_option = find_option_by_name(env_options, "host");
    argus_option_t *port_option = find_option_by_name(env_options, "port");
    argus_option_t *db_option = find_option_by_name(env_options, "database");
    argus_option_t *timeout_option = find_option_by_name(env_options, "timeout");
    
    cr_assert_eq(host_option->is_set, false, "Host option should not be set initially");
    cr_assert_eq(port_option->is_set, false, "Port option should not be set initially");
    cr_assert_eq(db_option->is_set, false, "Database option should not be set initially");
    cr_assert_eq(timeout_option->is_set, true, "Timeout option should be set due to default");
    cr_assert_eq(timeout_option->value.as_int, 30, "Timeout should have default value initially");
    
    // Load environment variables
    int status = load_env_vars(&argus);
    
    // Check that env vars were loaded successfully
    cr_assert_eq(status, ARGUS_SUCCESS, "Environment variables should load successfully");
    
    // Check that options were set from environment variables
    cr_assert_eq(host_option->is_set, true, "Host option should be set from env var");
    cr_assert_str_eq(host_option->value.as_string, "env-server.example.com", "Host value should be from env var");
    
    cr_assert_eq(port_option->is_set, true, "Port option should be set from env var");
    cr_assert_eq(port_option->value.as_int, 9000, "Port value should be from env var");
    
    cr_assert_eq(db_option->is_set, true, "Database option should be set from env var");
    cr_assert_str_eq(db_option->value.as_string, "postgres://user:pass@localhost/db", "Database value should be from env var");
    
    cr_assert_eq(timeout_option->is_set, true, "Timeout option should be set from env var");
    cr_assert_eq(timeout_option->value.as_int, 60, "Timeout value should be from env var");
    
    // Clean up
    argus_free(&argus);
}

// Test FLAG_ENV_OVERRIDE behavior
Test(env_vars, env_override_flag, .init = setup_env, .fini = teardown_env)
{
    // Prepare command line arguments that set all options
    char *argv[] = {
        "test_program", 
        "--host=cli-server.example.com", 
        "--port=8080", 
        "--database=mysql://localhost/db",
        "--timeout=45"
    };
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(env_options, "test_program", "1.0.0");
    argus.env_prefix = "TEST";
    
    // Parse command line arguments
    int status = argus_parse(&argus, argc, argv);
    
    // Check that parsing succeeded
    cr_assert_eq(status, ARGUS_SUCCESS, "Command line arguments should parse successfully");
    
    // Check option values
    cr_assert_str_eq(argus_get(&argus, "host").as_string, "cli-server.example.com", 
                     "Host should be from command line");
    
    cr_assert_eq(argus_get(&argus, "port").as_int, 8080, 
                 "Port should be from command line");
    
    cr_assert_str_eq(argus_get(&argus, "database").as_string, "mysql://localhost/db", 
                     "Database should be from command line");
    
    // Check that FLAG_ENV_OVERRIDE made env var override command line
    cr_assert_eq(argus_get(&argus, "timeout").as_int, 60, 
                 "Timeout should be from env var despite command line value");
    
    // Clean up
    argus_free(&argus);
}

// Test FLAG_AUTO_ENV behavior
Test(env_vars, auto_env_flag, .init = setup_env, .fini = teardown_env)
{
    // Test with a custom prefix
    argus_t argus = argus_init(env_options, "test_program", "1.0.0");
    argus.env_prefix = "CUSTOM";
    
    // Manually set up the environment with the custom prefix
    setenv("CUSTOM_PORT", "7777", 1);
    
    // Load environment variables
    int status = load_env_vars(&argus);
    
    // Check that env vars were loaded successfully
    cr_assert_eq(status, ARGUS_SUCCESS, "Environment variables should load successfully");
    
    // Check that auto-env option got the value from the custom prefix
    argus_option_t *port_option = find_option_by_name(env_options, "port");
    cr_assert_eq(port_option->is_set, true, "Port option should be set from auto-env");
    cr_assert_eq(port_option->value.as_int, 7777, "Port value should be from CUSTOM_PORT env var");
    
    // Clean up
    unsetenv("CUSTOM_PORT");
    argus_free(&argus);
}

// Test FLAG_NO_ENV_PREFIX behavior
Test(env_vars, no_env_prefix_flag, .init = setup_env, .fini = teardown_env)
{
    // Test with a non-prefixed variable
    argus_t argus = argus_init(env_options, "test_program", "1.0.0");
    argus.env_prefix = "TEST";
    
    // Manually set up the environment with and without prefix
    setenv("DATABASE_URL", "sqlite:///data.db", 1);  // No prefix
    setenv("TEST_DATABASE_URL", "mysql://wrong/db", 1);  // With prefix - should be ignored
    
    // Load environment variables
    int status = load_env_vars(&argus);
    
    // Check that env vars were loaded successfully
    cr_assert_eq(status, ARGUS_SUCCESS, "Environment variables should load successfully");
    
    // Check that NO_ENV_PREFIX option got the value from the unprefixed variable
    argus_option_t *db_option = find_option_by_name(env_options, "database");
    cr_assert_eq(db_option->is_set, true, "Database option should be set from env var");
    cr_assert_str_eq(db_option->value.as_string, "sqlite:///data.db", 
                     "Database value should be from unprefixed env var");
    
    // Clean up
    unsetenv("TEST_DATABASE_URL");
    argus_free(&argus);
}
