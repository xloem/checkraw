#pragma once

#include <stdio.h>
#include <stdlib.h>

#define check(result_error, compare, other) do {\
	void * result = (void *)(result_error); \
	if (!(result compare (void *)(other))) { \
		fprintf(stderr, #result_error " not " #compare #other ": failed as %p\n", result); \
		exit(((int)(result) == 0) ? -1 : (int)(result)); \
	} \
} while(0)
