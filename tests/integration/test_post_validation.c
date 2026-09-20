#include <criterion/criterion.h>
#include "argus.h"
#include "argus/internal/parsing.h"
#include "argus/errors.h"
#include <stdlib.h>
#include <string.h>

// Test options with dependencies and conflicts
ARGUS_OPTIONS(
    validation_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output"), CONFLICT("quiet")),
    OPTION_FLAG('q', "quiet", HELP("Quiet mode"), CONFLICT("verbose")),
    OPTION_STRING('u', "username", HELP("Username"), REQUIRE("password")),
    OPTION_STRING('p', "password", HELP("Password"), REQUIRE("username")),
    POSITIONAL_STRING("input", HELP("Input file")),
)

// Test options with exclusive groups
ARGUS_OPTIONS(
    group_options,
    HELP_OPTION(),
    GROUP_START("Compression", FLAGS(FLAG_EXCLUSIVE)),
        OPTION_FLAG('z', "gzip", HELP("Use gzip compression")),
        OPTION_FLAG('j', "bzip2", HELP("Use bzip2 compression")),
    GROUP_END(),
    POSITIONAL_STRING("input", HELP("Input file")),
)

// Regression test for #54: FLAG_REQUIRED on a non-positional option.
ARGUS_OPTIONS(
    required_option_options,
    HELP_OPTION(),
    OPTION_STRING('a', "algo", HELP("Algorithm"), FLAGS(FLAG_REQUIRED)),
    OPTION_STRING('o', "output", HELP("Output path")),
)

// Regression test for #64: a DEFAULT must not count as a user-provided value.
ARGUS_OPTIONS(
    default_conflict_options,
    HELP_OPTION(),
    OPTION_STRING('a', "alpha", HELP("Alpha"), DEFAULT("x"), CONFLICT("beta")),
    OPTION_STRING('b', "beta", HELP("Beta")),
)

ARGUS_OPTIONS(
    default_exclusive_options,
    HELP_OPTION(),
    GROUP_START("Mode", FLAGS(FLAG_EXCLUSIVE)),
        OPTION_INT('n', "num", HELP("Num"), DEFAULT(1)),
        OPTION_INT('m', "mum", HELP("Mum"), DEFAULT(2)),
    GROUP_END(),
)

ARGUS_OPTIONS(
    default_require_options,
    HELP_OPTION(),
    OPTION_STRING('u', "username", HELP("Username"), REQUIRE("password")),
    OPTION_STRING('p', "password", HELP("Password"), DEFAULT("secret")),
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

// Regression for #54: a missing FLAG_REQUIRED option must fail validation.
Test(post_validation, required_option_missing)
{
    char *argv[] = {"test_program", "-o", "out.bin"};
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(required_option_options, "test_program", "1.0.0");

    int status = parse_args(&argus, required_option_options, argc - 1, &argv[1]);
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");

    status = post_parse_validation(&argus);
    cr_assert_eq(status, ARGUS_ERROR_MISSING_REQUIRED,
                 "Validation should fail when a FLAG_REQUIRED option is missing");

    argus_free(&argus);
}

// Regression for #54: a present FLAG_REQUIRED option must pass validation.
Test(post_validation, required_option_present)
{
    char *argv[] = {"test_program", "-a", "RSA", "-o", "out.bin"};
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(required_option_options, "test_program", "1.0.0");

    int status = parse_args(&argus, required_option_options, argc - 1, &argv[1]);
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");

    status = post_parse_validation(&argus);
    cr_assert_eq(status, ARGUS_SUCCESS,
                 "Validation should succeed when FLAG_REQUIRED option is provided");

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

// Regression for #64: an option holding only its default must not trigger a conflict.
Test(post_validation, default_does_not_conflict)
{
    char *argv[] = {"test_program", "-b", "value"};
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(default_conflict_options, "test_program", "1.0.0");

    int status = parse_args(&argus, default_conflict_options, argc - 1, &argv[1]);
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");

    status = post_parse_validation(&argus);
    cr_assert_eq(status, ARGUS_SUCCESS,
                 "A default value must not conflict with a user-provided option");

    argus_free(&argus);
}

// Regression for #64: defaults in an exclusive group must not collide with each other.
Test(post_validation, defaults_in_exclusive_group)
{
    char *argv[] = {"test_program"};
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(default_exclusive_options, "test_program", "1.0.0");

    int status = parse_args(&argus, default_exclusive_options, argc - 1, &argv[1]);
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");

    status = post_parse_validation(&argus);
    cr_assert_eq(status, ARGUS_SUCCESS,
                 "Two defaults in an exclusive group must not be reported as a conflict");

    argus_free(&argus);
}

// Regression for #64: REQUIRE is satisfied by an option that carries a default.
Test(post_validation, require_satisfied_by_default)
{
    char *argv[] = {"test_program", "-u", "user123"};
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(default_require_options, "test_program", "1.0.0");

    int status = parse_args(&argus, default_require_options, argc - 1, &argv[1]);
    cr_assert_eq(status, ARGUS_SUCCESS, "Initial parsing should succeed");

    status = post_parse_validation(&argus);
    cr_assert_eq(status, ARGUS_SUCCESS,
                 "A required option holding a default always has a value");

    argus_free(&argus);
}
