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
 * Default configuration constants for display formatting
 */
#define DEFAULT_DESCRIPTION_COLUMN 32  // Column where descriptions start
#define DEFAULT_MAX_LINE_WIDTH     80  // Maximum width of a line
#define DEFAULT_OPTION_INDENT      2   // Indentation for options

/*
 * Default Smart Hint configuration
 */
#define DEFAULT_SMART_HINT_MAX_LENGTH 20     // Maximum length for short hints
#define DEFAULT_SMART_HINT_ALLOW_SPACES false // Allow spaces in short hints

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

/**
 * get_default_helper_config - Get default helper configuration
 *
 * @return  Default helper configuration
 */
argus_helper_config_t get_default_helper_config(void);


#endif /* ARGUS_INTERNAL_DISPLAY_H */
