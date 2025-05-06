#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <criterion/parameterized.h>
#include <time.h>
#include <stdio.h>
#include "argus.h"
#include "argus/errors.h"
#include "argus/internal/utils.h"

// Valid options definition for testing
ARGUS_OPTIONS(
    valid_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('c', "count", HELP("Counter value"), RANGE(1, 100), DEFAULT(1)),
    POSITIONAL_STRING("input", HELP("Input file"), FLAGS(FLAG_REQUIRED))
)

// Invalid options definition (missing help option - should fail validation)
ARGUS_OPTIONS(
    invalid_options,
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file"))
)

// Invalid options with duplicated option names
ARGUS_OPTIONS(
    duplicate_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('v', "verbose", HELP("Duplicate option"))  // Same names
)

// Helper function for measuring initialization time
double measure_init_time(argus_option_t *options, const char *program_name, 
                         const char *version, int iterations) 
{
    clock_t start, end;
    double total_time = 0.0;
    
    for (int i = 0; i < iterations; i++) {
        start = clock();
        
        // Initialize argus
        argus_t argus = argus_init(options, program_name, version);
        
        end = clock();
        total_time += ((double) (end - start)) / CLOCKS_PER_SEC;
        
        // Clean up
        argus_free(&argus);
    }
    
    return total_time / iterations;
}

#ifdef ARGUS_RELEASE
Test(release_mode, init_skips_validation_with_invalid_options)
{
    // With ARGUS_RELEASE defined, initialization should succeed even with invalid options
    argus_t argus = argus_init(invalid_options, "test_program", "1.0.0");
    
    // In release mode, the initialization shouldn't perform validation
    // so no errors should be reported
    cr_assert_eq(argus.error_stack.count, 0, 
                 "No errors should be reported in release mode");
                 
    // Even options with duplicate names should initialize without errors
    argus_t argus2 = argus_init(duplicate_options, "test_program", "1.0.0");
    cr_assert_eq(argus2.error_stack.count, 0, 
                 "No errors should be reported for duplicate options in release mode");
    
    argus_free(&argus);
    argus_free(&argus2);
}

Test(release_mode, parse_works_correctly_with_valid_options)
{
    // Test that parsing still works correctly in release mode
    argus_t argus = argus_init(valid_options, "test_program", "1.0.0");
    
    char *argv[] = {"test_program", "--verbose", "--output=test.txt", "input.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed in release mode");
    cr_assert_eq(argus_is_set(argus, "verbose"), true, "Verbose option should be set");
    cr_assert_str_eq(argus_get(argus, "output").as_string, "test.txt", "Output should be correctly set");
    cr_assert_str_eq(argus_get(argus, "input").as_string, "input.txt", "Input should be correctly set");
    
    argus_free(&argus);
}
#else
// In this test we expect the program to exit, so we don't need the assert_fail
// The .exit_code attribute will handle the expected behavior
Test(release_mode, init_performs_validation_with_invalid_options, .exit_code = 1, .init = cr_redirect_stderr)
{
    // This will exit due to validation errors, which will make the test pass
    // because of the .exit_code = 1 attribute
    argus_init(invalid_options, "test_program", "1.0.0");
    
    // We should never reach this point, but we need something here
    // to avoid compiler warnings about empty functions
    cr_assert(false, "Should not reach this point");
}

// Same here, the test passes if the program exits with the specified code
Test(release_mode, init_performs_validation_with_duplicate_options, .exit_code = 1, .init = cr_redirect_stderr)
{
    // This will exit due to validation errors, which will make the test pass
    argus_init(duplicate_options, "test_program", "1.0.0");
    
    // We should never reach this point
    cr_assert(false, "Should not reach this point");
}

Test(release_mode, init_succeeds_with_valid_options)
{
    // With validation enabled, initialization should succeed with valid options
    argus_t argus = argus_init(valid_options, "test_program", "1.0.0");
    
    // No errors should be reported for valid options
    cr_assert_eq(argus.error_stack.count, 0, 
                 "No errors should be reported for valid options");
    
    argus_free(&argus);
}
#endif

// This test is always run and measures performance
Test(release_mode, performance_comparison)
{
    // Number of iterations for reliable measurement
    const int iterations = 1000;
    
    // Measure initialization time with valid options
    double time_valid = measure_init_time(valid_options, "test_program", "1.0.0", iterations);
    
#ifdef ARGUS_RELEASE
    // In release mode, we can measure invalid options too (validation is skipped)
    double time_invalid = measure_init_time(invalid_options, "test_program", "1.0.0", iterations);
    double time_duplicate = measure_init_time(duplicate_options, "test_program", "1.0.0", iterations);
    
    cr_log_info("Performance in RELEASE mode (average over %d iterations):", iterations);
    cr_log_info("  Valid options: %.9f seconds", time_valid);
    cr_log_info("  Invalid options: %.9f seconds", time_invalid);
    cr_log_info("  Duplicate options: %.9f seconds", time_duplicate);
    
    // In release mode, all initializations should be roughly the same speed
    cr_assert_float_eq(time_valid, time_invalid, 0.0001, 
                     "Release mode should have similar performance for valid and invalid options");
#else
    cr_log_info("Performance in DEBUG mode (average over %d iterations):", iterations);
    cr_log_info("  Valid options: %.9f seconds", time_valid);
    
    // In debug mode, we can still assert that valid options initialize in a reasonable time
    cr_assert_lt(time_valid, 0.001, 
                "Even with validation, initialization should be reasonably fast for valid options");
#endif
}
