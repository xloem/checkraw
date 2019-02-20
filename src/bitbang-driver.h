#pragma once

#include <checkraw/bitbang.h>

typedef struct bitbang_driver_t bitbang_driver_t;

struct bitbang_driver_t
{
	bitbang_t device;
	void * handle;
	error_code_t (*init)(bitbang_driver_t *);
	void (*destroy)(bitbang_driver_t *);
	int (*pin_by_name)(bitbang_driver_t *, char const * name);
	char const * (*name_of_pin)(bitbang_driver_t *, unsigned pin);
	error_code_t (*mode)(bitbang_driver_t *, unsigned pin, int is_input, bitbang_feature_t);
	error_code_t (*write)(bitbang_driver_t *, unsigned pin, int isHigh);
	int (*read)(bitbang_driver_t *, unsigned pin);
};

bitbang_driver_t bitbang_sunxi_tools_driver();
