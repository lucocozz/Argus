#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/compiler.h"
#include "argus/internal/cross_platform.h"
#include "argus/internal/utils.h"
#include "argus/types.h"

argus_option_t *find_option_by_lname(argus_option_t *options, const char *lname)
{
    if (lname == NULL)
        return (NULL);
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_OPTION && options[i].lname &&
            strcmp(options[i].lname, lname) == 0)
            return (&options[i]);
    }
    return (NULL);
}

argus_option_t *find_option_by_sname(argus_option_t *options, char sname)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_OPTION && options[i].sname == sname)
            return (&options[i]);
    }
    return (NULL);
}

argus_option_t *find_positional(argus_option_t *options, int position)
{
    int pos_index = 0;

    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].type == TYPE_POSITIONAL) {
            if (pos_index == position)
                return (&options[i]);
            pos_index++;
        }
    }
    return (NULL);
}

argus_option_t *find_option_by_name(argus_option_t *options, const char *name)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        if (options[i].name && strcmp(options[i].name, name) == 0) {
            return (&options[i]);
        }
    }
    return (NULL);
}

const argus_option_t *get_active_options(argus_t *argus)
{
    if (argus->subcommand_depth > 0) {
        const argus_option_t *command = argus->subcommand_stack[argus->subcommand_depth - 1];
        if (command != NULL)
            return (command->sub_options);
    }
    return (argus->options);
}

static argus_option_t *find_from_relative_path(argus_t argus, const char *option_name)
{
    for (int i = (int)argus.subcommand_depth; i >= 0; --i) {
        argus_option_t *options;

        if (i == 0)
            options = argus.options;
        else
            options = argus.subcommand_stack[i - 1]->sub_options;

        argus_option_t *option = find_option_by_name(options, option_name);
        if (option != NULL)
            return (option);
    }
    return (NULL);
}

static size_t count_components(const char *path)
{
    size_t count = 0;

    for (const char *c = path; *c; ++c) {
        if (*c == '.')
            count++;
    }
    return (count);
}

argus_option_t *find_option_by_active_path(argus_t argus, const char *option_path)
{
    if (option_path == NULL)
        return (NULL);

    // Format: "option_name"
    if (strchr(option_path, '.') == NULL)
        return (find_from_relative_path(argus, option_path));

    // Format: ".option_name" (root)
    if (option_path[0] == '.')
        return (find_option_by_name(argus.options, option_path + 1));

    size_t component_count = count_components(option_path);
    if (component_count > argus.subcommand_depth)
        return (NULL);

    // Format: "subcommand.option_name"
    const char     *component = option_path;
    argus_option_t *options   = argus.options;
    for (size_t i = 0; i < component_count; ++i) {
        char *next_dot = strchr(component, '.');
        if (next_dot == NULL)
            break;

        const char *command          = argus.subcommand_stack[i]->name;
        size_t      component_lenght = next_dot - component;
        if (strncmp(component, command, component_lenght) != 0)
            return (NULL);

        component = next_dot + 1;
        options   = argus.subcommand_stack[i]->sub_options;
    }

    return (find_option_by_name(options, component));
}
