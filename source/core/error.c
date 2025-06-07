#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

#include <stdio.h>

const char *argus_strerror(argus_error_type_t error)
{
    switch (error) {
        case ARGUS_SUCCESS:
            return "Success";
        case ARGUS_SHOULD_EXIT:
            return "Should exit";
        case ARGUS_ERROR_DUPLICATE_OPTION:
            return "Duplicate option";
        case ARGUS_ERROR_INVALID_HANDLER:
            return "Invalid handler";
        case ARGUS_ERROR_INVALID_DEFAULT:
            return "Invalid default value";
        case ARGUS_ERROR_INVALID_GROUP:
            return "Invalid group";
        case ARGUS_ERROR_INVALID_DEPENDENCY:
            return "Invalid dependency";
        case ARGUS_ERROR_INVALID_FLAG:
            return "Invalid flag";
        case ARGUS_ERROR_INVALID_POSITION:
            return "Invalid position";
        case ARGUS_ERROR_INVALID_ARGUMENT:
            return "Invalid argument";
        case ARGUS_ERROR_MISSING_VALUE:
            return "Missing value";
        case ARGUS_ERROR_MISSING_REQUIRED:
            return "Missing required option";
        case ARGUS_ERROR_CONFLICTING_OPTIONS:
            return "Conflicting options";
        case ARGUS_ERROR_INVALID_FORMAT:
            return "Invalid format";
        case ARGUS_ERROR_EXCLUSIVE_GROUP:
            return "Exclusive group";
        case ARGUS_ERROR_INVALID_CHOICE:
            return "Invalid choice";
        case ARGUS_ERROR_INVALID_RANGE:
            return "Invalid range";
        case ARGUS_ERROR_NO_COMMAND:
            return "No command";
        case ARGUS_ERROR_INVALID_VALUE:
            return "Invalid value";
        case ARGUS_ERROR_MALFORMED_OPTION:
            return "Malformed option";
        case ARGUS_ERROR_MISSING_HELP:
            return "Missing help option";
        case ARGUS_ERROR_STACK_OVERFLOW:
            return "Error stack overflow";
        default:
            return "Unknown error";
    }
}

void argus_print_error_stack(const argus_t *argus)
{
    if (argus->error_stack.count == 0)
        return;

    fprintf(stderr, COLOR(ANSI_BOLD ANSI_RED, "Error stack (%zu errors):\n"),
            argus->error_stack.count);

    for (size_t i = 0; i < argus->error_stack.count && i < ARGUS_MAX_ERRORS_STACK; ++i) {
        const argus_error_t *error = &argus->error_stack.errors[i];

        fprintf(stderr, COLOR(ANSI_BOLD, "[%zu]"), i + 1);

        if (error->context.subcommand_name) {
            fprintf(stderr, " in subcommand ");
            fprintf(stderr, COLOR(ANSI_GREEN ANSI_BOLD, "'%s'"), error->context.subcommand_name);
        }

        if (error->context.group_name) {
            fprintf(stderr, " in group ");
            fprintf(stderr, COLOR(ANSI_MAGENTA ANSI_BOLD, "'%s'"), error->context.group_name);
        }

        if (error->context.option_name) {
            fprintf(stderr, " for option ");
            fprintf(stderr, COLOR(ANSI_CYAN ANSI_BOLD, "'%s'"), error->context.option_name);
        }
        fprintf(stderr, "\n");

        fprintf(stderr, COLOR(ANSI_BOLD, "\tError: "));
        fprintf(stderr, COLOR(ANSI_YELLOW ANSI_BOLD, "%s\n"), argus_strerror(error->code));

        if (error->message[0] != '\0') {
            fprintf(stderr, COLOR(ANSI_BOLD, "\tDetails: "));
            fprintf(stderr, COLOR(ANSI_ITALIC, "%s\n"), error->message);
        }
    }
    if (argus->error_stack.count >= ARGUS_MAX_ERRORS_STACK) {
        printf(COLOR(ANSI_ITALIC ANSI_RED, "(Too many errors, only the first %d are displayed)\n"),
               ARGUS_MAX_ERRORS_STACK);
    }
}

void argus_clear_errors(argus_t *argus)
{
    argus->error_stack.count = 0;
}

void argus_push_error(argus_t *argus, argus_error_t error)
{
    if (argus->error_stack.count >= ARGUS_MAX_ERRORS_STACK) {
        argus_error_t *last = &argus->error_stack.errors[ARGUS_MAX_ERRORS_STACK - 1];
        last->context       = (argus_error_context_t){0};
        last->code          = ARGUS_ERROR_STACK_OVERFLOW;
        snprintf(last->message, ARGUS_MAX_ERROR_MESSAGE_SIZE, "Too many errors");
        return;
    }

    argus_error_t *last = &argus->error_stack.errors[argus->error_stack.count++];
    *last               = error;
}
