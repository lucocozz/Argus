#include "argus/errors.h"
#include "argus/internal/utils.h"
#include "argus/types.h"
#include <stddef.h>
#include <string.h>

argus_value_t argus_get(argus_t *argus, const char *option_path)
{
    argus->error_code       = 0;
    argus_option_t *option = find_option_by_active_path(*argus, option_path);
    if (option == NULL) {
        argus->error_code = ARGUS_ERROR_NO_VALUE;
        return ((argus_value_t){.raw = 0});
    }
    return (option->value);
}

bool argus_is_set(argus_t *argus, const char *option_path)
{
    argus_option_t *option = find_option_by_active_path(*argus, option_path);
    if (option == NULL)
        return (false);
    return (option->is_set);
}

size_t argus_count(argus_t *argus, const char *option_path)
{
    argus_option_t *option = find_option_by_active_path(*argus, option_path);
    if (option == NULL)
        return (0);
    return (option->value_count);
}

argus_value_t argus_array_get(argus_t *argus, const char *option_path, size_t index)
{
    argus->error_code       = 0;
    argus_option_t *option = find_option_by_active_path(*argus, option_path);

    if (option == NULL) {
        argus->error_code = ARGUS_ERROR_NO_VALUE;
        return ((argus_value_t){.raw = 0});
    }

    // Check if the option is an array type
    if (!(option->value_type & VALUE_TYPE_ARRAY)) {
        argus->error_code = ARGUS_ERROR_INVALID_TYPE;
        return ((argus_value_t){.raw = 0});
    }

    // Check if the index is valid
    if (index >= option->value_count) {
        argus->error_code = ARGUS_ERROR_INVALID_INDEX;
        return ((argus_value_t){.raw = 0});
    }

    // Return the element at the specified index
    return option->value.as_array[index];
}

argus_value_t argus_map_get(argus_t *argus, const char *option_path, const char *key)
{
    argus->error_code       = 0;
    argus_option_t *option = find_option_by_active_path(*argus, option_path);

    if (option == NULL) {
        argus->error_code = ARGUS_ERROR_NO_VALUE;
        return ((argus_value_t){.raw = 0});
    }

    // Check if the option is a map type
    if (!(option->value_type & VALUE_TYPE_MAP)) {
        argus->error_code = ARGUS_ERROR_INVALID_TYPE;
        return ((argus_value_t){.raw = 0});
    }

    // Look for the key in the map
    for (size_t i = 0; i < option->value_count; ++i) {
        if (option->value.as_map[i].key && strcmp(option->value.as_map[i].key, key) == 0) {
            return option->value.as_map[i].value;
        }
    }

    // Key not found
    argus->error_code = ARGUS_ERROR_INVALID_KEY;
    return ((argus_value_t){.raw = 0});
}

argus_array_it_t argus_array_it(argus_t *argus, const char *option_path)
{
    argus_array_it_t it     = {0};
    argus_option_t  *option = find_option_by_active_path(*argus, option_path);

    if (option == NULL || !(option->value_type & VALUE_TYPE_ARRAY))
        return it;  // Return empty iterator

    it._array    = option->value.as_array;
    it._count    = option->value_count;
    it._position = 0;
    return it;
}

bool argus_array_next(argus_array_it_t *it)
{
    if (it == NULL || it->_position >= it->_count)
        return false;

    it->value = it->_array[it->_position++];
    return true;
}

void argus_array_reset(argus_array_it_t *it)
{
    if (it != NULL)
        it->_position = 0;
}

argus_map_it_t argus_map_it(argus_t *argus, const char *option_path)
{
    argus_map_it_t  it     = {0};
    argus_option_t *option = find_option_by_active_path(*argus, option_path);

    if (option == NULL || !(option->value_type & VALUE_TYPE_MAP))
        return it;  // Return empty iterator

    it._map      = option->value.as_map;
    it._count    = option->value_count;
    it._position = 0;
    return it;
}

bool argus_map_next(argus_map_it_t *it)
{
    if (it == NULL || it->_position >= it->_count)
        return false;

    argus_pair_t pair = it->_map[it->_position++];

    it->key   = pair.key;
    it->value = pair.value;

    return (true);
}

void argus_map_reset(argus_map_it_t *it)
{
    if (it != NULL)
        it->_position = 0;
}
