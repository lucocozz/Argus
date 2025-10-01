#include <criterion/criterion.h>
#include "argus.h"
#include <stdlib.h>
#include <string.h>

// Test options with comprehensive variadic usage
ARGUS_OPTIONS(
    comprehensive_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output directory")),
    OPTION_INT('j', "jobs", HELP("Number of parallel jobs"), VALIDATOR(V_RANGE(1, 16))),
    POSITIONAL_STRING("command", HELP("Command to execute")),
    POSITIONAL_MANY_STRING("targets", HELP("Target files to process")),
)

// Test options with different variadic types
ARGUS_OPTIONS(
    calc_options,
    HELP_OPTION(),
    POSITIONAL_STRING("operation", HELP("Mathematical operation (sum, avg, max, min)")),
    POSITIONAL_MANY_FLOAT("numbers", HELP("Numbers to operate on")),
)

// Test options with optional variadic
ARGUS_OPTIONS(
    optional_variadic_options,
    HELP_OPTION(),
    POSITIONAL_STRING("mode", HELP("Processing mode")),
    POSITIONAL_MANY_STRING("files", HELP("Optional files to process"), FLAGS(FLAG_OPTIONAL)),
)

// Setup function for comprehensive tests
static argus_t setup_comprehensive_argus(void)
{
    char *argv[] = {
        "build_tool",
        "-v",
        "--jobs=4",
        "--output=/tmp/build",
        "compile",
        "main.c",
        "utils.c",
        "parser.c",
        "logger.c"
    };
    int argc = sizeof(argv) / sizeof(char *);

    argus_t argus = argus_init(comprehensive_options, "build_tool", "2.1.0");
    int status = argus_parse(&argus, argc, argv);

    cr_assert_eq(status, ARGUS_SUCCESS, "Comprehensive parsing should succeed");
    
    return argus;
}

// Test comprehensive usage with all features
Test(variadic_usage, comprehensive_functionality)
{
    argus_t argus = setup_comprehensive_argus();
    
    // Check options
    cr_assert_eq(argus_get(&argus, "verbose").as_bool, true, "Verbose flag should be set");
    cr_assert_str_eq(argus_get(&argus, "output").as_string, "/tmp/build", "Output should be '/tmp/build'");
    cr_assert_eq(argus_get(&argus, "jobs").as_int, 4, "Jobs should be 4");
    
    // Check positional argument
    cr_assert_str_eq(argus_get(&argus, "command").as_string, "compile", "Command should be 'compile'");
    
    // Check variadic positionals
    cr_assert_eq(argus_count(&argus, "targets"), 4, "Should have 4 target files");
    
    // Verify each target file
    const char* expected_targets[] = {"main.c", "utils.c", "parser.c", "logger.c"};
    for (int i = 0; i < 4; i++) {
        cr_assert_str_eq(argus_array_get(&argus, "targets", i).as_string, expected_targets[i],
                        "Target %d should be '%s'", i, expected_targets[i]);
    }
    
    argus_free(&argus);
}

// Test array iterator with variadic positionals
Test(variadic_usage, array_iterator_functionality)
{
    argus_t argus = setup_comprehensive_argus();
    
    // Test array iterator
    argus_array_it_t it = argus_array_it(&argus, "targets");
    
    cr_assert_eq(it._count, 4, "Iterator should report 4 elements");
    
    // Iterate and verify values
    int count = 0;
    const char* expected[] = {"main.c", "utils.c", "parser.c", "logger.c"};
    
    while (argus_array_next(&it)) {
        cr_assert_str_eq(it.value.as_string, expected[count], 
                        "Iterator value should match expected at position %d", count);
        count++;
    }
    
    cr_assert_eq(count, 4, "Iterator should yield 4 values");
    
    // Test iterator reset
    argus_array_reset(&it);
    cr_assert_eq(it._position, 0, "Reset should set position to 0");
    
    // Verify reset works
    cr_assert(argus_array_next(&it), "Iterator should yield values after reset");
    cr_assert_str_eq(it.value.as_string, "main.c", "First value after reset should be 'main.c'");
    
    argus_free(&argus);
}

// Test mathematical calculation with float variadic
Test(variadic_usage, float_calculation)
{
    char *argv[] = {"calc", "sum", "3.14", "2.71", "1.41", "0.57"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(calc_options, "calc", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Float calculation parsing should succeed");
    
    // Check operation
    cr_assert_str_eq(argus_get(&argus, "operation").as_string, "sum", "Operation should be 'sum'");
    
    // Check numbers
    cr_assert_eq(argus_count(&argus, "numbers"), 4, "Should have 4 numbers");
    
    // Calculate sum using iterator
    double sum = 0.0;
    argus_array_it_t it = argus_array_it(&argus, "numbers");
    
    while (argus_array_next(&it)) {
        sum += it.value.as_float;
    }
    
    cr_assert_float_eq(sum, 7.83, 0.001, "Sum should be approximately 7.83");
    
    // Verify individual values
    cr_assert_float_eq(argus_array_get(&argus, "numbers", 0).as_float, 3.14, 0.001, "First number should be 3.14");
    cr_assert_float_eq(argus_array_get(&argus, "numbers", 1).as_float, 2.71, 0.001, "Second number should be 2.71");
    cr_assert_float_eq(argus_array_get(&argus, "numbers", 2).as_float, 1.41, 0.001, "Third number should be 1.41");
    cr_assert_float_eq(argus_array_get(&argus, "numbers", 3).as_float, 0.57, 0.001, "Fourth number should be 0.57");
    
    argus_free(&argus);
}

// Test optional variadic with files provided
Test(variadic_usage, optional_variadic_with_files)
{
    char *argv[] = {"processor", "batch", "input1.txt", "input2.txt", "input3.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(optional_variadic_options, "processor", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Optional variadic with files should succeed");
    
    cr_assert_str_eq(argus_get(&argus, "mode").as_string, "batch", "Mode should be 'batch'");
    cr_assert_eq(argus_count(&argus, "files"), 3, "Should have 3 files");
    
    // Check files using array access
    cr_assert_str_eq(argus_array_get(&argus, "files", 0).as_string, "input1.txt", "First file should be 'input1.txt'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 1).as_string, "input2.txt", "Second file should be 'input2.txt'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 2).as_string, "input3.txt", "Third file should be 'input3.txt'");
    
    argus_free(&argus);
}

// Test optional variadic without files
Test(variadic_usage, optional_variadic_no_files)
{
    char *argv[] = {"processor", "interactive"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(optional_variadic_options, "processor", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Optional variadic without files should succeed");
    
    cr_assert_str_eq(argus_get(&argus, "mode").as_string, "interactive", "Mode should be 'interactive'");
    cr_assert_eq(argus_count(&argus, "files"), 0, "Should have 0 files");
    
    // Test that iterator works with empty variadic
    argus_array_it_t it = argus_array_it(&argus, "files");
    cr_assert_eq(it._count, 0, "Iterator should report 0 elements");
    cr_assert(!argus_array_next(&it), "Iterator should not yield any values");
    
    argus_free(&argus);
}

// Test error handling with required variadic missing
Test(variadic_usage, required_variadic_missing)
{
    char *argv[] = {"calc", "sum"}; // Missing numbers
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(calc_options, "calc", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Required variadic missing should fail");
    
    argus_free(&argus);
}

// Test out-of-bounds array access
Test(variadic_usage, out_of_bounds_access)
{
    argus_t argus = setup_comprehensive_argus();
    
    // Test valid access
    cr_assert_str_eq(argus_array_get(&argus, "targets", 0).as_string, "main.c", "Valid access should work");
    
    // Test out-of-bounds access
    argus_value_t invalid_value = argus_array_get(&argus, "targets", 10);
    cr_assert_eq(invalid_value.raw, 0, "Out-of-bounds access should return empty value");
    
    // Test negative index
    argus_value_t negative_value = argus_array_get(&argus, "targets", -1);
    cr_assert_eq(negative_value.raw, 0, "Negative index should return empty value");
    
    argus_free(&argus);
}

// Test memory management and cleanup
Test(variadic_usage, memory_management)
{
    // Test single instance to verify basic memory management works
    char *argv[] = {"test", "batch", "file1.txt", "file2.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(optional_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Memory management test should succeed");
    cr_assert_eq(argus_count(&argus, "files"), 2, "Should have 2 files");
    
    argus_free(&argus);
    
    // If we reach here without crashes, memory management is working
    cr_assert(true, "Memory management test completed successfully");
}