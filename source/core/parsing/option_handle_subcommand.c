#include "argus/internal/parsing.h"
#include "argus/types.h"

int handle_subcommand(argus_t *argus, argus_option_t *option, int argc, char **argv)
{
    option->is_set = true;
    argus->subcommand_stack[argus->subcommand_depth] = option;
    argus->subcommand_depth++;
    return parse_args(argus, option->sub_options, argc, argv);
}
