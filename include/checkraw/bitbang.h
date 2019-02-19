#pragma once

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
	void * handle;
} bitbang_t;

