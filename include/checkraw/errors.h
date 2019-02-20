#pragma once

typedef enum
{
	ERROR_SUCCESS = 0,
	ERROR_OPEN_FAILURE = -1,
	ERROR_SETUP_FAILURE = -2,
	ERROR_INVALID_STATE = -3,
	ERROR_INVALID_ARG = -4
} error_code_t;

typedef struct
{
	error_code_t code;
	char const * task;
	char const * result;
	int subcode;
} error_t;

error_code_t error_raise(error_code_t code, char const * task, char const * result, int subcode);
error_code_t error_errno_raise(error_code_t code, char const * task);
error_t * error_last();
char const * error_descr(error_t * error);

