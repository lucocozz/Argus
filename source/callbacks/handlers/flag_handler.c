#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

int flag_handler(argus_t *argus, argus_option_t *option, char *arg)
{
    UNUSED(argus);
    UNUSED(arg);
    option->value.as_bool = !option->value.as_bool;
    return (ARGUS_SUCCESS);
}
