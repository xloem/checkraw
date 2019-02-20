#include "../bitbang-driver.h"

/* embed the sunxi-tools pio sourcecode, but drop the main function */
#define main __dummy_bitbang_sunxi_main
#include "../../submodules/sunxi-tools/pio.c"
#undef main

#define PIO_PINS_PER_PORT 32
#define PIO_PIN_CT (PIO_PINS_PER_PORT * PIO_NR_PORTS)

static checkraw_error bitbang_sunxi_tools_init(bitbang_driver_t * dev)
{
	char * buf;
	int pagesize = sysconf(_SC_PAGESIZE);
	int addr = 0x01c20800 & ~(pagesize-1);
	int offset = 0x01c20800 & (pagesize-1);
	int fd = open("/dev/mem",O_RDWR);

	if (-1 == fd)
	{
		return cr_errno(CRE_OPEN_FAILURE, "opening /dev/mem");
	}
	buf = mmap(NULL, (0x800 + pagesize - 1) & ~(pagesize-1), PROT_WRITE|PROT_READ, MAP_SHARED, fd, addr);
	close(fd);
	if (MAP_FAILED == buf)
	{
		return cr_errno(CRE_OPEN_FAILURE, "mmaping pio region");
	}
	buf += offset;

	dev->handle = buf;
	return CRE_SUCCESS;
}

static void bitbang_sunxi_tools_destroy(bitbang_driver_t * dev)
{
	int pagesize = sysconf(_SC_PAGESIZE);
	int offset = 0x01c20800 & (pagesize-1);
	char * buf = dev->handle;
	buf -= offset;
	munmap(dev->handle, (0x800 + pagesize - 1) & ~(pagesize-1));
	dev->handle = NULL;
}

inline static checkraw_error pin2portpin(bitbang_driver_t * dev, unsigned pin, unsigned * port, unsigned * portpin)
{
	if (dev->handle == NULL) 
	{
		return cr_error(CRE_INVALID_STATE, "sunxi gpio use", "device handle is NULL", 0);
	}
	if (pin > PIO_PIN_CT)
	{
		return cr_error(CRE_INVALID_ARG, "sunxi gpio use", "invalid pin", pin);
	}
	*port = pin / PIO_PINS_PER_PORT;
	*portpin = pin % PIO_PINS_PER_PORT;
	return CRE_SUCCESS;
}

static int bitbang_sunxi_tools_pin_by_name(bitbang_driver_t * dev, char const * name)
{
	size_t len = strlen(name);
	unsigned port, pin;
	if (len < 3 || len > 4 || (name[0] != 'P' && name[0] != 'p'))
	{
		return cr_error(CRE_INVALID_ARG, "pin_by_name", "sunxi pins are named P<bank><##> like PB17", len);
	}
	/* '(c & 31) - 1' converts a letter to an int in a case-insensitive way */
	port = (name[1] & 31) - 1;
	pin = name[2] - '0';
	if (len > 3)
	{
		pin = pin * 10 + (name[3] - '0');
	}

	pin += port * PIO_PINS_PER_PORT;
	if (pin > PIO_PIN_CT)
	{
		return cr_error(CRE_INVALID_ARG, "pin_by_name", "pin out of range", pin);
	}

	return pin;
}

static char const * bitbang_sunxi_tools_name_of_pin(bitbang_driver_t * dev, unsigned pin)
{
	static char ret[5] = "PA00";
	unsigned port;
	checkraw_error result = pin2portpin(dev, pin, &port, &pin);
	if (result != CRE_SUCCESS)
	{
		return NULL;
	}
	ret[1] = 'A' + port;
	ret[2] = '0' + pin / 10;
	ret[3] = '0' + (pin % 10);
	return ret;
}

static checkraw_error bitbang_sunxi_tools_mode(bitbang_driver_t * dev, unsigned pin, int is_input, bitbang_feature_t pull)
{
	struct pio_status pio;
	unsigned port;
	checkraw_error result = pin2portpin(dev, pin, &port, &pin);
	if (result != CRE_SUCCESS)
	{
		return result;
	}
	pio.mul_sel = is_input ? 0 : 1;
	switch (pull)
	{
	case BF_PULL_FLOAT:
		pio.pull = 0;
		break;
	case BF_PULL_UP:
		pio.pull = 1;
		break;
	case BF_PULL_DOWN:
		pio.pull = 2;
		break;
	default:
		pio.pull = -1;
		break;
	}
	pio.drv_level = is_input ? 0 : 1;
	pio.data = -1;
	pio_set(dev->handle, port, pin, &pio);
	return CRE_SUCCESS;
}

static int bitbang_sunxi_tools_read(bitbang_driver_t * dev, unsigned pin)
{
	struct pio_status pio;
	unsigned port;
	checkraw_error result = pin2portpin(dev, pin, &port, &pin);
	if (result != CRE_SUCCESS)
	{
		return result;
	}
	pio_get(dev->handle, port, pin, &pio);
	return pio.data;
}

static checkraw_error bitbang_sunxi_tools_write(bitbang_driver_t * dev, unsigned pin, int isHigh)
{
	struct pio_status pio;
	unsigned port;
	checkraw_error result = pin2portpin(dev, pin, &port, &pin);
	if (result != CRE_SUCCESS)
	{
		return result;
	}
	pio.mul_sel = -1;
	pio.pull = -1;
	pio.drv_level = -1;
	pio.data = isHigh;
	pio_set(dev->handle, port, pin, &pio);
	return CRE_SUCCESS;
}

bitbang_driver_t bitbang_sunxi_tools_driver()
{
	bitbang_driver_t driver;
	driver.device.name = "sunxi-tools pio";
	driver.device.pin_ct = PIO_PIN_CT;
	driver.device.features = BF_PULL_FLOAT | BF_PULL_UP | BF_PULL_DOWN;
	driver.handle = NULL;
	driver.init = bitbang_sunxi_tools_init;
	driver.destroy = bitbang_sunxi_tools_destroy;
	driver.pin_by_name = bitbang_sunxi_tools_pin_by_name;
	driver.name_of_pin = bitbang_sunxi_tools_name_of_pin;
	driver.mode = bitbang_sunxi_tools_mode;
	driver.write = bitbang_sunxi_tools_write;
	driver.read = bitbang_sunxi_tools_read;
	return driver;
}
