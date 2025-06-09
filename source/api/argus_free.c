#include <stdlib.h>

#include "argus/internal/utils.h"
#include "argus/types.h"

static void free_options(argus_option_t *options)
{
    for (argus_option_t *option = options; option->type != TYPE_NONE; ++option)
        free_option_value(option);
}

void argus_free(argus_t *argus)
{
    free_options(argus->options);
    for (size_t i = 0; i < argus->subcommand_depth; ++i) {
        const argus_option_t *subcommand = argus->subcommand_stack[i];
        argus_option_t       *options    = subcommand->sub_options;
        free_options(options);
    }
}
