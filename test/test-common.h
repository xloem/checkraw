#pragma once

#include <stdio.h>
#include <stdlib.h>

#define check(result_error, compare, other) do \
{\
	error_last()->code = ERROR_SUCCESS; \
	void * result = (void *)(result_error); \
	if (!(result compare (void *)(other))) \
	{ \
		if (error_last()->code != ERROR_SUCCESS) \
		{ \
			fprintf(stderr, "%s\n", error_descr(error_last())); \
		} \
		fprintf(stderr, #result_error " not " #compare #other ": failed as %p\n", result); \
		exit(((int)(result) == 0) ? -1 : (int)(result)); \
	} \
} while(0)
