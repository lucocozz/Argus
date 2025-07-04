/*
 * MIT License
 *
 * Copyright (c) 2025 lucocozz
 *
 * This file is part of Argus.
 * See LICENSE file in the project root for full license information.
 */

#ifndef ARGUS_INTERNAL_CALLBACKS_VALIDATORS_H
#define ARGUS_INTERNAL_CALLBACKS_VALIDATORS_H

#include "argus/types.h"

/**
 * range_validator - Validate that a numeric value is within a specified range
 *
 * @param argus      Argus context
 * @param option_ptr Pointer to option (cast to argus_option_t*)
 * @param data       Validator data containing range information
 *
 * @return Status code (0 for success, non-zero for error)
 */
int range_validator(argus_t *argus, void *option_ptr, validator_data_t data);

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
