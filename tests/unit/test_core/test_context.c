#include <criterion/criterion.h>
#include "argus/internal/context.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include "argus/options.h"
#include "argus/errors.h"

// Mock options for testing
ARGUS_OPTIONS(
    context_test_options,
    OPTION_FLAG('v', "verbose", HELP("Verbose output")),
    OPTION_STRING('o', "output", HELP("Output file")),
)

// Mock subcommand options
ARGUS_OPTIONS(
    context_sub_options,
    OPTION_FLAG('d', "debug", HELP("Debug mode")),
    OPTION_STRING('f', "file", HELP("File option")),
)

// Mock nested subcommand options
ARGUS_OPTIONS(
    context_nested_options,
    OPTION_FLAG('n', "nested", HELP("Nested flag")),
)

// Define subcommands with nested structure
static argus_option_t sub_command = {
    .type = TYPE_SUBCOMMAND,
    .name = "sub",
    .sub_options = context_sub_options
};

static argus_option_t nested_command = {
    .type = TYPE_SUBCOMMAND,
    .name = "nested",
    .sub_options = context_nested_options
};

// Mock group for testing
static argus_option_t test_group = {
    .type = TYPE_GROUP,
    .name = "test_group",
    .help = "Test group description"
};

// Static argus context
static argus_t test_argus;

// Setup function
void context_setup(void)
{
    test_argus.program_name = "test_program";
    test_argus.options = context_test_options;
    test_argus.error_stack.count = 0;
    context_init(&test_argus);
}

Test(context, context_init, .init = context_setup)
{
    // Verify initial state
    cr_assert_null(test_argus.context.option, "Option context should be NULL initially");
    cr_assert_null(test_argus.context.group, "Group context should be NULL initially");
    cr_assert_eq(test_argus.context.subcommand_depth, 0, "Subcommand stack should be empty initially");
    
    // Check that all subcommand stack entries are NULL
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++) {
        cr_assert_null(test_argus.context.subcommand_stack[i], "Subcommand stack entries should be NULL");
    }
}

Test(context, context_set_option, .init = context_setup)
{
    // Find an option to set
    argus_option_t* verbose_option = find_option_by_name(context_test_options, "verbose");
    cr_assert_not_null(verbose_option, "Should find test option");
    
    // Set the option context
    context_set_option(&test_argus, verbose_option);
    
    // Verify it was set correctly
    cr_assert_eq(test_argus.context.option, verbose_option->name, "Option context should be set to option name");
    
    // Try with another option
    argus_option_t* output_option = find_option_by_name(context_test_options, "output");
    context_set_option(&test_argus, output_option);
    
    cr_assert_eq(test_argus.context.option, output_option->name, "Option context should be updated to new option name");
    
    // Unset the option context
    context_unset_option(&test_argus);
    cr_assert_null(test_argus.context.option, "Option context should be NULL after unsetting");
}

Test(context, context_set_group, .init = context_setup)
{
    // Set the group context
    context_set_group(&test_argus, &test_group);
    
    // Verify it was set correctly
    cr_assert_eq(test_argus.context.group, test_group.name, "Group context should be set to group name");
    
    // Unset the group context
    context_unset_group(&test_argus);
    cr_assert_null(test_argus.context.group, "Group context should be NULL after unsetting");
}

Test(context, subcommand_stack_operations, .init = context_setup)
{
    // Initial state
    cr_assert_eq(test_argus.context.subcommand_depth, 0, "Initial subcommand depth should be 0");
    cr_assert_null(context_get_subcommand(&test_argus), "Initial active subcommand should be NULL");
    
    // Push first subcommand
    context_push_subcommand(&test_argus, &sub_command);
    cr_assert_eq(test_argus.context.subcommand_depth, 1, "Depth should be 1 after pushing");
    cr_assert_eq(context_get_subcommand(&test_argus), &sub_command, "Active subcommand should be first subcommand");
    
    // Push second subcommand
    context_push_subcommand(&test_argus, &nested_command);
    cr_assert_eq(test_argus.context.subcommand_depth, 2, "Depth should be 2 after pushing again");
    cr_assert_eq(context_get_subcommand(&test_argus), &nested_command, "Active subcommand should be second subcommand");
    
    // Pop subcommands
    const argus_option_t* popped = context_pop_subcommand(&test_argus);
    cr_assert_eq(popped, &nested_command, "Popped value should be second subcommand");
    cr_assert_eq(test_argus.context.subcommand_depth, 1, "Depth should be 1 after popping");
    cr_assert_eq(context_get_subcommand(&test_argus), &sub_command, "Active subcommand should be first subcommand");
    
    popped = context_pop_subcommand(&test_argus);
    cr_assert_eq(popped, &sub_command, "Popped value should be first subcommand");
    cr_assert_eq(test_argus.context.subcommand_depth, 0, "Depth should be 0 after popping");
    cr_assert_null(context_get_subcommand(&test_argus), "Active subcommand should be NULL");
    
    // Popping empty stack
    popped = context_pop_subcommand(&test_argus);
    cr_assert_null(popped, "Popping empty stack should return NULL");
    cr_assert_eq(test_argus.context.subcommand_depth, 0, "Depth should remain 0");
}

Test(context, subcommand_stack_overflow, .init = context_setup)
{
    // Push to max depth
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++) {
        context_push_subcommand(&test_argus, &sub_command);
    }
    
    cr_assert_eq(test_argus.context.subcommand_depth, MAX_SUBCOMMAND_DEPTH, "Depth should be at maximum");
    
    // Try to push one more (should not increase depth)
    context_push_subcommand(&test_argus, &nested_command);
    
    cr_assert_eq(test_argus.context.subcommand_depth, MAX_SUBCOMMAND_DEPTH, "Depth should remain at maximum");
    cr_assert_eq(test_argus.error_stack.count, 1, "Error should be reported on stack overflow");
    
    // Clean up
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++) {
        context_pop_subcommand(&test_argus);
    }
}

Test(context, get_error_context, .init = context_setup)
{
    // Set up a complete context
    argus_option_t* option = find_option_by_name(context_test_options, "verbose");
    context_set_option(&test_argus, option);
    context_set_group(&test_argus, &test_group);
    context_push_subcommand(&test_argus, &sub_command);
    
    // Get the error context
    argus_error_context_t ctx = get_error_context(&test_argus);
    
    // Verify all fields
    cr_assert_eq(ctx.option_name, option->name, "Option name should be correct in error context");
    cr_assert_eq(ctx.group_name, test_group.name, "Group name should be correct in error context");
    cr_assert_eq(ctx.subcommand_name, sub_command.name, "Subcommand name should be correct in error context");
    
    // Clean up
    context_unset_option(&test_argus);
    context_unset_group(&test_argus);
    context_pop_subcommand(&test_argus);
    
    // Get empty context
    ctx = get_error_context(&test_argus);
    
    // Verify all fields are NULL
    cr_assert_null(ctx.option_name, "Option name should be NULL in empty error context");
    cr_assert_null(ctx.group_name, "Group name should be NULL in empty error context");
    cr_assert_null(ctx.subcommand_name, "Subcommand name should be NULL in empty error context");
}
