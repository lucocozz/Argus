/**
 * argus/internal/callbacks/validators.h - Internal validators implementation
 *
 * INTERNAL HEADER - NOT PART OF THE PUBLIC API
 * This header defines standard validator callbacks for different validation types.
 *
 * MIT License - Copyright (c) 2024 lucocozz
 */

#ifndef ARGUS_INTERNAL_CALLBACKS_VALIDATORS_H
#define ARGUS_INTERNAL_CALLBACKS_VALIDATORS_H

#include "argus/types.h"

/**
 * range_validator - Validate that a numeric value is within a specified range
 *
 * @param argus  Argus context
 * @param value  Value to validate
 * @param data   Validator data containing range information
 *
 * @return Status code (0 for success, non-zero for error)
 */
int range_validator(argus_t *argus, argus_option_t *option, validator_data_t data);

/**
 * regex_validator - Validate that a string value matches a regex pattern
 *
 * @param argus  Argus context
 * @param value  Value to validate
 * @param data   Validator data containing regex information
 *
 * @return Status code (0 for success, non-zero for error)
 */
int regex_validator(argus_t *argus, const char *value, validator_data_t data);

#endif /* ARGUS_INTERNAL_CALLBACKS_VALIDATORS_H */
