#include <criterion/criterion.h>
#include "argus.h"
#include "argus/internal/parsing.h"
#include "argus/errors.h"
#include <stdlib.h>
#include <string.h>

// Test options with dependencies and conflicts
ARGUS_OPTIONS(
    validation_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    OPTION_FLAG('v', "verbose", HELP("Verbose output"), CONFLICTS("quiet")),
    OPTION_FLAG('q', "quiet", HELP("Quiet mode"), CONFLICTS("verbose")),
    OPTION_STRING('u', "username", HELP("Username"), REQUIRES("password")),
    OPTION_STRING('p', "password", HELP("Password"), REQUIRES("username")),
    POSITIONAL_STRING("input", HELP("Input file"), FLAGS(FLAG_REQUIRED))
)

// Test options with exclusive groups
ARGUS_OPTIONS(
    group_options,
    HELP_OPTION(FLAGS(FLAG_EXIT)),
    GROUP_START("Compression", GROUP_DESC("Compression options"), FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('z', "gzip", HELP("Use gzip compression")),
        OPTION_FLAG('j', "bzip2", HELP("Use bzip2 compression")),
    GROUP_END(),
    POSITIONAL_STRING("input", HELP("Input file"), FLAGS(FLAG_REQUIRED))
)

// Test post_parse_validation with required positionals
Test(post_validation, required_positional)
{
    char *argv[] = {"test_program", "-v"};  // Missing required input file
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(validation_options, "test_program", "1.0.0");
    
    // Parse arguments (but don't run post_parse_validation yet)
    int status = parse_args(&argus, validation_options, argc - 1, &argv[1]);
    
    // Parsing itself should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");
    
    // Now run post validation
    status = post_parse_validation(&argus);
    
    // Validation should fail due to missing positional
    cr_assert_neq(status, ARGUS_SUCCESS, "Validation should fail due to missing required positional");
    
    // Clean up
    argus_free(&argus);
}

// Test post_parse_validation with dependency requirements
Test(post_validation, option_dependencies)
{
    // Test with one option but missing its dependency
    char *argv[] = {"test_program", "-u", "user123", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(validation_options, "test_program", "1.0.0");
    
    // Parse arguments
    int status = parse_args(&argus, validation_options, argc - 1, &argv[1]);
    
    // Parsing itself should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");
    
    // Run post validation
    status = post_parse_validation(&argus);
    
    // Validation should fail due to missing dependency
    cr_assert_neq(status, ARGUS_SUCCESS, "Validation should fail due to missing dependency");
    
    // Clean up
    argus_free(&argus);
}

// Test post_parse_validation with conflicts
Test(post_validation, option_conflicts)
{
    // Test with conflicting options
    char *argv[] = {"test_program", "-v", "-q", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(validation_options, "test_program", "1.0.0");
    
    // Parse arguments
    int status = parse_args(&argus, validation_options, argc - 1, &argv[1]);
    
    // Parsing itself should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");
    
    // Run post validation
    status = post_parse_validation(&argus);
    
    // Validation should fail due to conflicting options
    cr_assert_neq(status, ARGUS_SUCCESS, "Validation should fail due to conflicting options");
    
    // Clean up
    argus_free(&argus);
}

// Test post_parse_validation with exclusive groups
Test(post_validation, exclusive_groups)
{
    // Test with multiple options from an exclusive group
    char *argv[] = {"test_program", "-z", "-j", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(group_options, "test_program", "1.0.0");
    
    // Parse arguments
    int status = parse_args(&argus, group_options, argc - 1, &argv[1]);
    
    // Parsing itself should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");
    
    // Run post validation
    status = post_parse_validation(&argus);
    
    // Validation should fail due to exclusive group violation
    cr_assert_neq(status, ARGUS_SUCCESS, "Validation should fail due to exclusive group violation");
    
    // Clean up
    argus_free(&argus);
}

// Test post_parse_validation with valid inputs
Test(post_validation, valid_inputs)
{
    // Test with all requirements met
    char *argv[] = {"test_program", "-u", "user123", "-p", "pass456", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(validation_options, "test_program", "1.0.0");
    
    // Parse arguments
    int status = parse_args(&argus, validation_options, argc - 1, &argv[1]);
    
    // Parsing should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");
    
    // Run post validation
    status = post_parse_validation(&argus);
    
    // Validation should succeed
    cr_assert_eq(status, ARGUS_SUCCESS, "Validation should succeed with valid inputs");
    
    // Clean up
    argus_free(&argus);
}
