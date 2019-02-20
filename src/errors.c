#include <checkraw/errors.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>

static error_t last_error;

error_code_t error_raise(error_code_t code, char const * task, char const * result, int subcode)
{
	last_error.code = code;
	last_error.task = task;
	last_error.result = result;
	last_error.subcode = subcode;
	return code;
}

error_code_t error_errno_raise(error_code_t code, char const * task)
{
	return error_raise(code, task, strerror(errno), errno);
}

error_t * error_last()
{
	return &last_error;
}

char const * error_descr(error_t * error)
{
	static char buf[4096];
	static char * errorstrs[] = {
		"SUCCESS",
		"OPEN FAILURE",
		"SETUP FAILURE",
		"INVALID STATE",
		"INVALID ARG"
	};
	unsigned idx = -error->code;
	if (idx < sizeof(errorstrs) / sizeof(errorstrs[0]))
	{
		snprintf(buf, sizeof(buf), "%s: task=\"%s\" result=\"%s\" subcode=%d", errorstrs[idx], error->task, error->result, error->subcode);
	}
	else
	{
		snprintf(buf, sizeof(buf), "CORRUPT ERROR: code=%d task=\"%s\" result=\"%s\" subcode=%d", error->code, error->task, error->result, error->subcode);
	}
	return buf;
}

