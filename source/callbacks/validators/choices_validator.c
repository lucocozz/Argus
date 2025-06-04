#include "argus/types.h"
#include "argus/errors.h"

#include "string.h"

int choices_string_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
	argus_option_t *option = (argus_option_t *)option_ptr;
	choices_data_t *choices = &data.choices;

	for (size_t i = 0; i < choices->count; i++) {
		if (strcmp(option->value.as_string, choices->as_strings[i]) == 0)
			return ARGUS_SUCCESS;
	}

	ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
	                   "Value '%s' is not one of the choices", option->value.as_string);
}

int choices_int_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
	argus_option_t *option = (argus_option_t *)option_ptr;
	choices_data_t *choices = &data.choices;

	for (size_t i = 0; i < choices->count; i++) {
		if (option->value.as_int == choices->as_ints[i])
			return ARGUS_SUCCESS;
	}

	ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
	                   "Value '%lld' is not one of the choices", option->value.as_int);
}

int choices_float_validator(argus_t *argus, void *option_ptr, validator_data_t data)
{
	argus_option_t *option = (argus_option_t *)option_ptr;
	choices_data_t *choices = &data.choices;

	for (size_t i = 0; i < choices->count; i++) {
		if (option->value.as_float == choices->as_floats[i])
			return ARGUS_SUCCESS;
	}

	ARGUS_REPORT_ERROR(argus, ARGUS_ERROR_INVALID_CHOICE,
	                   "Value '%f' is not one of the choices", option->value.as_float);
}

