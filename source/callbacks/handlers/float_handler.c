#include <stdlib.h>

#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/options.h"
#include "argus/types.h"

int float_handler(argus_t *argus, argus_option_t *option, char *value)
{
    UNUSED(argus);
    option->value.as_float = strtof(value, NULL);
    return (ARGUS_SUCCESS);
}
