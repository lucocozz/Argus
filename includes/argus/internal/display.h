/**
 * argus/internal/display.h - Internal display functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines internal functions for displaying help, usage, and version info.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_DISPLAY_H
#define ARGUS_INTERNAL_DISPLAY_H

#include "argus/types.h"

/*
 * Configuration constants for display formatting
 */
#define DESCRIPTION_COLUMN 32  // Column where descriptions start
#define MAX_LINE_WIDTH     80  // Maximum width of a line
#define OPTION_INDENT      2   // Indentation for options

/**
 * display_help - Display help information
 *
 * @param argus  	Argus context
 * @param command   Specific subcommand to display help for, or NULL for general help
 */
void display_help(argus_t *argus, const argus_option_t *command);

/**
 * display_usage - Display short usage information
 *
 * @param argus  	Argus context
 * @param command   Specific subcommand to display usage for, or NULL for general usage
 */
void display_usage(argus_t *argus, const argus_option_t *command);

/**
 * display_version - Display version information
 *
 * @param argus  Argus context
 */
void display_version(argus_t *argus);

#endif /* ARGUS_INTERNAL_DISPLAY_H */
