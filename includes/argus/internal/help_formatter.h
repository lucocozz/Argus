/**
 * argus/internal/help_formatter.h - Help formatting functions
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines functions for formatting help text, option names, and descriptions.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_HELP_FORMATTER_H
#define ARGUS_INTERNAL_HELP_FORMATTER_H

#include "argus/internal/help_organizer.h"
#include "argus/types.h"

/**
 * Get base type name for value hints
 *
 * @param type  Value type
 * @return      Type name string
 */
const char *get_base_type_name(argus_valtype_t type);

/**
 * Get collection format pattern
 *
 * @param type  Value type
 * @return      Format pattern or NULL if not a collection
 */
const char *get_collection_format(argus_valtype_t type);

/**
 * Format a collection hint
 *
 * @param format     Format pattern
 * @param type_name  Base type name
 * @return           Formatted hint string (static buffer)
 */
char *format_collection_hint(const char *format, const char *type_name);

/**
 * Print value hint for an option
 *
 * @param option  Option to print hint for
 */
void print_value_hint(const argus_option_t *option);

/**
 * Print wrapped text with proper indentation
 *
 * @param text        Text to print
 * @param indent      Base indentation
 * @param line_width  Maximum line width
 */
void print_wrapped_text(const char *text, size_t indent, size_t line_width);

/**
 * Print option name (short and long forms with hints)
 *
 * @param option  Option to print name for
 * @param indent  Indentation level
 * @return        Total width of printed name
 */
size_t print_option_name(const argus_option_t *option, size_t indent);

/**
 * Build description text for an option (including defaults, attributes, etc.)
 *
 * @param option  Option to build description for
 * @return        Allocated description string - caller must free
 */
char *build_option_description(const argus_option_t *option);

/**
 * Print option description with proper alignment and wrapping
 *
 * @param option   Option to print description for
 * @param padding  Padding before description
 */
void print_option_description(const argus_option_t *option, size_t padding);

#endif /* ARGUS_INTERNAL_HELP_FORMATTER_H */