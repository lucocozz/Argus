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
