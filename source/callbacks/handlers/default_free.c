#include <stdlib.h>

#include "argus/errors.h"
#include "argus/types.h"

int default_free(argus_option_t *option)
{
    free(option->value.as_ptr);
    return (ARGUS_SUCCESS);
}
