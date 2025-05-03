#include "argus/errors.h"
#include "argus/types.h"
#include <stdlib.h>

void context_init_subcommands(argus_t *argus)
{
    for (size_t i = 0; i < MAX_SUBCOMMAND_DEPTH; i++)
        argus->context.subcommand_stack[i] = NULL;
    argus->context.subcommand_depth = 0;
}

const argus_option_t *context_get_subcommand(argus_t *argus)
{
    if (argus->context.subcommand_depth == 0)
        return (NULL);
    return argus->context.subcommand_stack[argus->context.subcommand_depth - 1];
}

void context_push_subcommand(argus_t *argus, const argus_option_t *cmd)
{
    if (argus->context.subcommand_depth >= MAX_SUBCOMMAND_DEPTH) {
        ARGUS_COLLECT_ERROR(argus, ARGUS_ERROR_STACK_OVERFLOW, "Subcommand stack overflow");
        return;
    }
    argus->context.subcommand_stack[argus->context.subcommand_depth++] = cmd;
}

const argus_option_t *context_pop_subcommand(argus_t *argus)
{
    if (argus->context.subcommand_depth == 0)
        return (NULL);

    const argus_option_t *cmd = argus->context.subcommand_stack[--argus->context.subcommand_depth];
    argus->context.subcommand_stack[argus->context.subcommand_depth] = NULL;
    return (cmd);
}

void context_set_option(argus_t *argus, argus_option_t *option)
{
    argus->context.option = option->name;
}

void context_unset_option(argus_t *argus)
{
    argus->context.option = NULL;
}

void context_set_group(argus_t *argus, argus_option_t *group)
{
    argus->context.group = group->name;
}

void context_unset_group(argus_t *argus)
{
    argus->context.group = NULL;
}

argus_error_context_t get_error_context(argus_t *argus)
{
    const argus_option_t *subcommand = context_get_subcommand(argus);

    argus_error_context_t context = {.option_name     = argus->context.option,
                                     .group_name      = argus->context.group,
                                     .subcommand_name = subcommand ? subcommand->name : NULL};
    return (context);
}

void context_init(argus_t *argus)
{
    argus->context.option = NULL;
    argus->context.group  = NULL;
    context_init_subcommands(argus);
}
