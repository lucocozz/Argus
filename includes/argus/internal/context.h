/**
 * argus/internal/context.h - Internal context management
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for managing the internal context
 * state during parsing.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_CONTEXT_H
#define ARGUS_INTERNAL_CONTEXT_H

#include "argus/types.h"

/**
 * context_init - Initialize the context
 *
 * @param argus  Argus context
 */
void context_init(argus_t *argus);

/**
 * Subcommand stack management
 */
void                  context_init_subcommands(argus_t *argus);
const argus_option_t *context_get_subcommand(argus_t *argus);
void                  context_push_subcommand(argus_t *argus, const argus_option_t *cmd);
const argus_option_t *context_pop_subcommand(argus_t *argus);

/**
 * Option context management
 */
void context_set_option(argus_t *argus, argus_option_t *option);
void context_unset_option(argus_t *argus);

/**
 * Group context management
 */
void context_set_group(argus_t *argus, argus_option_t *group);
void context_unset_group(argus_t *argus);

#endif /* ARGUS_INTERNAL_CONTEXT_H */
