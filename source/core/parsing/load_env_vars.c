#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "argus/errors.h"
#include "argus/internal/parsing.h"
#include "argus/types.h"

/**
 * get_env_var_name - Get environment variable name for an option
 *
 * @param argus  Argus context
 * @param option Option to get env var name for
 *
 * @return Environment variable name or NULL if none
 */
static const char *get_env_var_name(argus_t *argus, argus_option_t *option)
{
    static char full_name[128];
    const char *prefix           = argus->env_prefix ? argus->env_prefix : "";
    size_t      prefix_len       = strlen(prefix);
    bool        needs_underscore = prefix_len > 0 && prefix[prefix_len - 1] != '_';

    if (option->env_name) {
        if (option->flags & FLAG_NO_ENV_PREFIX)
            return (option->env_name);

        if (argus->env_prefix && strncmp(option->env_name, argus->env_prefix, prefix_len) == 0)
            return (option->env_name);

        if (needs_underscore) {
            snprintf(full_name, sizeof(full_name), "%s_%s", prefix, option->env_name);
        } else {
            snprintf(full_name, sizeof(full_name), "%s%s", prefix, option->env_name);
        }
        return (full_name);
    }

    if (option->flags & FLAG_AUTO_ENV) {
        const char *name = option->name ? option->name : (option->lname ? option->lname : "");

        if (option->flags & FLAG_NO_ENV_PREFIX) {
            snprintf(full_name, sizeof(full_name), "%s", name);
        } else if (needs_underscore) {
            snprintf(full_name, sizeof(full_name), "%s_%s", prefix, name);
        } else {
            snprintf(full_name, sizeof(full_name), "%s%s", prefix, name);
        }

        for (char *p = full_name; *p; ++p)
            *p = *p == '-' ? '_' : toupper(*p);
        return (full_name);
    }

    return (NULL);
}

static int load_env(argus_t *argus, argus_option_t *options)
{
    for (int i = 0; options[i].type != TYPE_NONE; ++i) {
        argus_option_t *option = &options[i];

        if (option->type == TYPE_GROUP || option->type == TYPE_SUBCOMMAND)
            continue;

        if (option->is_set && !(option->flags & FLAG_ENV_OVERRIDE))
            continue;

        const char *env_name = get_env_var_name(argus, option);
        if (!env_name)
            continue;

        char *env_value = getenv(env_name);
        if (!env_value)
            continue;

        bool          was_set   = option->is_set;
        argus_value_t old_value = option->value;

        //! possible leak here
        int status = execute_callbacks(argus, option, env_value);
        if (status != ARGUS_SUCCESS) {
            if (was_set) {
                option->is_set = was_set;
                option->value  = old_value;
            }
            return (status);
        }
    }
    return (ARGUS_SUCCESS);
}

/**
 * load_environment_variables - Load option values from environment variables
 *
 * @param argus   Argus context
 * @param options Options array to process
 *
 * @return Status code (0 for success, non-zero for error)
 */
int load_env_vars(argus_t *argus)
{
    int status = load_env(argus, argus->options);
    if (status != ARGUS_SUCCESS)
        return (status);

    for (size_t i = 0; i < argus->context.subcommand_depth; ++i) {
        const argus_option_t *subcommand = argus->context.subcommand_stack[i];
        if (subcommand && subcommand->sub_options) {
            int status = load_env(argus, subcommand->sub_options);
            if (status != ARGUS_SUCCESS)
                return (status);
        }
    }
    return (ARGUS_SUCCESS);
}
