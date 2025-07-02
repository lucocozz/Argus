/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#include "argus/types.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void free_option_value(argus_option_t *option)
{
    if (option->is_allocated == false)
        return;

    if (option->free_handler != NULL)
        option->free_handler(option);
    else
        free(option->value.as_ptr);
    option->is_allocated = false;
    option->value.as_ptr = NULL;
}

argus_value_t choices_to_value(argus_valtype_t type, argus_value_t choices, size_t choices_count,
                               size_t index)
{
    argus_value_t value = {0};

    if (index >= choices_count)
        return value;

    switch (type) {
        case VALUE_TYPE_INT:
            value.as_int64 = choices.as_array_int[index];
            break;
        case VALUE_TYPE_STRING:
            value.as_string = choices.as_array_string[index];
            break;
        case VALUE_TYPE_FLOAT:
            value.as_float = choices.as_array_float[index];
            break;
        default:
            break;
    }
    return (value);
}

int cmp_value(argus_valtype_t type, const argus_value_t a, const argus_value_t b)
{
    if (a.raw == b.raw)
        return 0;

    switch (type) {
        case VALUE_TYPE_FLAG:
            return a.as_bool - b.as_bool;
        case VALUE_TYPE_INT:
            return (int)(a.as_int - b.as_int);
        case VALUE_TYPE_STRING:
            if (a.as_string == NULL || b.as_string == NULL)
                return -1;
            return strcmp(a.as_string, b.as_string);
        case VALUE_TYPE_FLOAT:
            if (a.as_float == b.as_float)
                return 0;
            else if (a.as_float > b.as_float)
                return 1;
            else
                return -1;
        case VALUE_TYPE_BOOL:
            return a.as_bool - b.as_bool;
        default:
            return -1;
    }
}

void print_value(FILE *stream, argus_valtype_t type, argus_value_t value)
{
    switch (type) {
        case VALUE_TYPE_FLAG:
            fprintf(stream, "%s", value.as_bool ? "true" : "false");
            break;
        case VALUE_TYPE_INT:
            fprintf(stream, "%d", value.as_int);
            break;
        case VALUE_TYPE_STRING:
            fprintf(stream, "\"%s\"", value.as_string);
            break;
        case VALUE_TYPE_FLOAT:
            fprintf(stream, "%f", value.as_float);
            break;
        case VALUE_TYPE_BOOL:
            fprintf(stream, "%s", value.as_bool ? "true" : "false");
            break;
        default:
            break;
    }
}

void print_value_array(FILE *stream, argus_valtype_t type, argus_value_t *values, size_t count)
{
    fprintf(stream, "[");
    for (size_t i = 0; i < count; ++i) {
        print_value(stream, type, values[i]);
        if (i < count - 1)
            fprintf(stream, ", ");
    }
    fprintf(stream, "]");
}
