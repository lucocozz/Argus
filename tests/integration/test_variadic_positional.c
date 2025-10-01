#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"
#include "argus/errors.h"

// Test options with basic variadic positional
ARGUS_OPTIONS(
    basic_variadic_options,
    HELP_OPTION(),
    POSITIONAL_STRING("cmd", HELP("Command name")),
    POSITIONAL_MANY_STRING("files", HELP("Files to process")),
)

// Test options with different types of variadic positionals
ARGUS_OPTIONS(
    typed_variadic_options,
    HELP_OPTION(),
    POSITIONAL_MANY_INT("numbers", HELP("Numbers to process")),
)

// Test options mixing regular options with variadic positionals
ARGUS_OPTIONS(
    mixed_options,
    HELP_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Verbose mode")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input", HELP("Input file")),
    POSITIONAL_MANY_STRING("targets", HELP("Target files")),
)

// Test options with optional variadic positionals
ARGUS_OPTIONS(
    optional_variadic_options,
    HELP_OPTION(),
    POSITIONAL_STRING("cmd", HELP("Command name")),
    POSITIONAL_MANY_STRING("files", HELP("Files to process"), FLAGS(FLAG_OPTIONAL)),
)

// Helper function for output redirection
void setup_error_redirect(void)
{
    cr_redirect_stderr();
}

// Test basic variadic string positional
Test(variadic_positional, basic_string_variadic)
{
    char *argv[] = {"test", "build", "file1.c", "file2.c", "file3.c"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(basic_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with variadic positionals");
    cr_assert_str_eq(argus_get(&argus, "cmd").as_string, "build", "Command should be 'build'");
    
    // Check variadic arguments
    cr_assert_eq(argus_count(&argus, "files"), 3, "Should have 3 files");
    cr_assert_str_eq(argus_array_get(&argus, "files", 0).as_string, "file1.c", "First file should be 'file1.c'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 1).as_string, "file2.c", "Second file should be 'file2.c'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 2).as_string, "file3.c", "Third file should be 'file3.c'");
    
    argus_free(&argus);
}

// Test variadic int positional
Test(variadic_positional, basic_int_variadic)
{
    char *argv[] = {"test", "10", "20", "30", "40"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(typed_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with int variadic positionals");
    
    // Check variadic arguments
    cr_assert_eq(argus_count(&argus, "numbers"), 4, "Should have 4 numbers");
    cr_assert_eq(argus_array_get(&argus, "numbers", 0).as_int, 10, "First number should be 10");
    cr_assert_eq(argus_array_get(&argus, "numbers", 1).as_int, 20, "Second number should be 20");
    cr_assert_eq(argus_array_get(&argus, "numbers", 2).as_int, 30, "Third number should be 30");
    cr_assert_eq(argus_array_get(&argus, "numbers", 3).as_int, 40, "Fourth number should be 40");
    
    argus_free(&argus);
}

// Test mixing options with variadic positionals
Test(variadic_positional, mixed_with_options)
{
    char *argv[] = {"test", "-v", "--output=result.txt", "input.txt", "target1.txt", "target2.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(mixed_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with mixed options and variadic positionals");
    
    // Check options
    cr_assert_eq(argus_get(&argus, "verbose").as_bool, true, "Verbose flag should be set");
    cr_assert_str_eq(argus_get(&argus, "output").as_string, "result.txt", "Output should be 'result.txt'");
    
    // Check positionals
    cr_assert_str_eq(argus_get(&argus, "input").as_string, "input.txt", "Input should be 'input.txt'");
    
    // Check variadic arguments
    cr_assert_eq(argus_count(&argus, "targets"), 2, "Should have 2 targets");
    cr_assert_str_eq(argus_array_get(&argus, "targets", 0).as_string, "target1.txt", "First target should be 'target1.txt'");
    cr_assert_str_eq(argus_array_get(&argus, "targets", 1).as_string, "target2.txt", "Second target should be 'target2.txt'");
    
    argus_free(&argus);
}

// Test using -- separator with variadic positionals
Test(variadic_positional, with_separator)
{
    char *argv[] = {"test", "build", "--", "--verbose", "-output"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(basic_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with -- separator");
    cr_assert_str_eq(argus_get(&argus, "cmd").as_string, "build", "Command should be 'build'");
    
    // Check that option-like arguments after -- are treated as positional
    cr_assert_eq(argus_count(&argus, "files"), 2, "Should have 2 files");
    cr_assert_str_eq(argus_array_get(&argus, "files", 0).as_string, "--verbose", "First file should be '--verbose'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 1).as_string, "-output", "Second file should be '-output'");
    
    argus_free(&argus);
}

// Test optional variadic positionals with arguments
Test(variadic_positional, optional_with_args)
{
    char *argv[] = {"test", "process", "file1.txt", "file2.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(optional_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with optional variadic positionals");
    cr_assert_str_eq(argus_get(&argus, "cmd").as_string, "process", "Command should be 'process'");
    
    // Check variadic arguments
    cr_assert_eq(argus_count(&argus, "files"), 2, "Should have 2 files");
    cr_assert_str_eq(argus_array_get(&argus, "files", 0).as_string, "file1.txt", "First file should be 'file1.txt'");
    cr_assert_str_eq(argus_array_get(&argus, "files", 1).as_string, "file2.txt", "Second file should be 'file2.txt'");
    
    argus_free(&argus);
}

// Test optional variadic positionals without arguments
Test(variadic_positional, optional_without_args)
{
    char *argv[] = {"test", "process"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(optional_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed with optional variadic positionals (no args)");
    cr_assert_str_eq(argus_get(&argus, "cmd").as_string, "process", "Command should be 'process'");
    
    // Check that no variadic arguments were provided
    cr_assert_eq(argus_count(&argus, "files"), 0, "Should have 0 files");
    
    argus_free(&argus);
}

// Test required variadic positionals missing arguments
Test(variadic_positional, required_missing_args, .init = setup_error_redirect)
{
    char *argv[] = {"test", "build"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(basic_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_neq(status, ARGUS_SUCCESS, "Parsing should fail with missing required variadic positionals");
    
    argus_free(&argus);
}

// Test array iterator with variadic positionals
Test(variadic_positional, array_iterator)
{
    char *argv[] = {"test", "process", "a.txt", "b.txt", "c.txt"};
    int argc = sizeof(argv) / sizeof(char *);
    
    argus_t argus = argus_init(basic_variadic_options, "test", "1.0.0");
    int status = argus_parse(&argus, argc, argv);
    
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing should succeed");
    
    // Test iterator functionality
    argus_array_it_t it = argus_array_it(&argus, "files");
    cr_assert_eq(it._count, 3, "Iterator should report 3 elements");
    
    int count = 0;
    const char* expected[] = {"a.txt", "b.txt", "c.txt"};
    
    while (argus_array_next(&it)) {
        cr_assert_str_eq(it.value.as_string, expected[count], "Iterator value should match expected");
        count++;
    }
    
    cr_assert_eq(count, 3, "Iterator should yield 3 values");
    
    argus_free(&argus);
}