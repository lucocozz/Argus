#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <criterion/parameterized.h>
#include <time.h>
#include <stdio.h>
#include "argus.h"
#include "argus/errors.h"
#include "argus/internal/utils.h"


// Invalid options definition (missing help option - should fail validation)
ARGUS_OPTIONS(
    invalid_options,
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
    POSITIONAL_STRING("input"),
)

#ifndef ARGUS_RELEASE

Test(release_mode, perform_validation, .exit_code = 1, .init = cr_redirect_stderr)
{
    argus_t argus = argus_init(invalid_options, "test_program", "1.0.0");

    // In release mode, the library should not validate options
    cr_assert_gt(argus.error_stack.count, 0,
                 "Errors should be reported in release mode");

    argus_free(&argus);
}

#else

Test(release_mode, skip_validation)
{
    argus_t argus = argus_init(invalid_options, "test_program", "1.0.0");

    // In debug mode, the library should validate options
    cr_assert_eq(argus.error_stack.count, 0, 
                 "Errors should be reported in debug mode");

    argus_free(&argus);
}
#endif

