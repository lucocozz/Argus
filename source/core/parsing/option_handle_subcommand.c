#include "argus/internal/context.h"
#include "argus/internal/parsing.h"
#include "argus/types.h"

int handle_subcommand(argus_t *argus, argus_option_t *option, int argc, char **argv)
{
    context_push_subcommand(argus, option);
    option->is_set = true;
    return parse_args(argus, option->sub_options, argc, argv);
}
