#pragma once

#include <checkraw/errors.h>
#include <stddef.h>

typedef enum
{
	BF_NONE = 0,
	BF_PULL_FLOAT = 1<<0,
	BF_PULL_UP =    1<<1,
	BF_PULL_DOWN =  1<<2
} bitbang_feature_t;

typedef struct
{
	unsigned pin_ct;
	unsigned features;
	char const * name;
} bitbang_t;

size_t bitbang_nr_drivers();
bitbang_t * bitbang_driver(size_t idx);

checkraw_error bitbang_init(bitbang_t * dev);
void bitbang_destroy(bitbang_t * dev);

int bitbang_pin_by_name(bitbang_t * dev, char const * name);
const char * bitbang_name_of_pin(bitbang_t * dev, unsigned pin);

checkraw_error bitbang_mode(bitbang_t * dev, unsigned pin, int is_input, bitbang_feature_t pull);
checkraw_error bitbang_write(bitbang_t * dev, unsigned pin, int isHigh);
int bitbang_read(bitbang_t * dev, unsigned pin);
