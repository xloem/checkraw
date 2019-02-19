#pragma once

#include <checkraw/bitbang.h>
#include <checkraw/errors.h>

checkraw_error bitbang_sunxi_init(bitbang_t * dev);

void bitbang_sunxi_destroy(bitbang_t * dev);

checkraw_error bitbang_sunxi_mode(bitbang_t * dev, unsigned pin, int is_input, bitbang_feature_t pull);

checkraw_error bitbang_sunxi_write(bitbang_t * dev, unsigned pin, int isHigh);

int bitbang_sunxi_read(bitbang_t * dev, unsigned pin);
