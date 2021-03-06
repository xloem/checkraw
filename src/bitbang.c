#include "bitbang-driver.h"

error_code_t bitbang_init(bitbang_t * dev)
{
	if (dev == NULL)
	{
		return error_raise(ERROR_INVALID_STATE, "bitbang_init", "device is NULL", 0);
	}
	return ((bitbang_driver_t*)dev)->init((bitbang_driver_t*)dev);
}

void bitbang_destroy(bitbang_t * dev)
{
	if (dev == NULL)
	{
		return;
	}
	((bitbang_driver_t*)dev)->destroy((bitbang_driver_t*)dev);
}

int bitbang_pin_by_name(bitbang_t * dev, char const * name)
{
	if (dev == NULL)
	{
		return error_raise(ERROR_INVALID_STATE, "bitbang_pin_by_name", "device is NULL", 0);
	}
	return ((bitbang_driver_t*)dev)->pin_by_name((bitbang_driver_t*)dev, name);
}

char const * bitbang_name_of_pin(bitbang_t * dev, unsigned pin)
{
	if (dev == NULL)
	{
		return NULL;
	}
	return ((bitbang_driver_t*)dev)->name_of_pin((bitbang_driver_t*)dev, pin);
}

error_code_t bitbang_mode(bitbang_t * dev, unsigned pin, int is_input, bitbang_feature_t pull)
{
	if (dev == NULL)
	{
		return error_raise(ERROR_INVALID_STATE, "bitbang_mode", "device is NULL", 0);
	}
	return ((bitbang_driver_t*)dev)->mode((bitbang_driver_t*)dev, pin, is_input, pull);
}

error_code_t bitbang_write(bitbang_t * dev, unsigned pin, int isHigh)
{
	if (dev == NULL)
	{
		return error_raise(ERROR_INVALID_STATE, "bitbang_write", "device is NULL", 0);
	}
	return ((bitbang_driver_t*)dev)->write((bitbang_driver_t*)dev, pin, isHigh);
}

int bitbang_read(bitbang_t * dev, unsigned pin)
{
	if (dev == NULL)
	{
		return error_raise(ERROR_INVALID_STATE, "bitbang_read", "device is NULL", 0);
	}
	return ((bitbang_driver_t*)dev)->read((bitbang_driver_t*)dev, pin);
}

static bitbang_driver_t(*bitbang_driver_factories[])() =
{
	bitbang_sunxi_tools_driver
};

static bitbang_driver_t bitbang_drivers[sizeof(bitbang_driver_factories) / sizeof(*bitbang_driver_factories)];

static void init()
{
	static int initialized = 0;
	size_t i;

	if (initialized)
	{
		return;
	}

	for (i = 0; i < bitbang_nr_drivers(); ++ i)
	{
		bitbang_drivers[i] = bitbang_driver_factories[i]();
	}
	initialized = 1;
}

size_t bitbang_nr_drivers()
{
	return sizeof(bitbang_drivers) / sizeof(bitbang_driver_t);
}

bitbang_t * bitbang_driver(size_t idx)
{
	init();
	return &bitbang_drivers[idx].device;
}
