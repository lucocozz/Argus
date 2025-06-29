#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "argus.h"

// Basic usage example options
ARGUS_OPTIONS(
    basic_options,
    HELP_OPTION(),
    VERSION_OPTION(),
    OPTION_FLAG('v', "verbose", HELP("Enable verbose output")),
    OPTION_STRING('o', "output", HELP("Output file"), DEFAULT("output.txt")),
    OPTION_INT('p', "port", HELP("Port number"), DEFAULT(8080), VALIDATOR(V_RANGE(1, 65535))),
    POSITIONAL_STRING("input", HELP("Input file")),
)

// Test successful parsing and execution
Test(basic_usage, successful_parse)
{
    // Initialize arguments
    char *argv[] = {
        "program",
        "--verbose",
        "--output=custom.txt",
        "--port=9000",
        "input.txt"
    };
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    // Initialize argus
    argus_t argus = argus_init(basic_options, "program", "1.0.0");
    
    // Parse arguments
    int status = argus_parse(&argus, argc, argv);
    
    // Assert parsing succeeded
    cr_assert_eq(status, ARGUS_SUCCESS, "Parsing failed with status %d", status);
    
    // Check values
    cr_assert_eq(argus_get(&argus, "verbose").as_bool, true);
    cr_assert_str_eq(argus_get(&argus, "output").as_string, "custom.txt");
    cr_assert_eq(argus_get(&argus, "port").as_int, 9000);
    cr_assert_str_eq(argus_get(&argus, "input").as_string, "input.txt");
    
    // Clean up
    argus_free(&argus);
}

// Test help display
Test(basic_usage, display_help, .init = cr_redirect_stdout)
{
    // Initialize arguments
    char *argv[] = {"program", "--help"};
    int argc = sizeof(argv) / sizeof(argv[0]);
	(void)argc;

    // Initialize argus
    argus_t argus = argus_init(basic_options, "program", "1.0.0");
    argus.description = "Test program description";

	argus_parse(&argus, argc, argv);

    // We expect this to exit, but we'll catch the exit with Criterion's special assertion
    cr_assert_fail(); // Should not reach this line
}

// Helper function to set up error testing
void setup_error_case(void)
{
    // Redirect stderr to capture error messages
    cr_redirect_stderr();
}

// Test required argument missing
Test(basic_usage, missing_required, .init = setup_error_case)
{
    // Initialize arguments (missing required positional arg)
    char *argv[] = {"program", "--verbose"};
    int argc = sizeof(argv) / sizeof(argv[0]);
    
    // Initialize argus
    argus_t argus = argus_init(basic_options, "program", "1.0.0");
    
    // Parse arguments
    int status = argus_parse(&argus, argc, argv);
    
    // Assert parsing failed with appropriate error
    cr_assert_neq(status, ARGUS_SUCCESS);
    
    // Clean up
    argus_free(&argus);
}
