#pragma once

typedef enum {
	CRE_SUCCESS = 0,
	CRE_OPEN_FAILURE = -1,
	CRE_SETUP_FAILURE = -2,
	CRE_INVALID_STATE = -3,
	CRE_INVALID_ARG = -4,
} checkraw_error;

inline checkraw_error cr_error(checkraw_error code, char const * task, char const * result, int subcode)
{
	/* TODO: copy strings into static storage for reporting */
	return code;
}

inline checkraw_error cr_errno(checkraw_error code, char const * task)
{
	/* TODO: turn errno into a result string */
	return cr_error(code, task, "STUB errno unused", 0);
}
