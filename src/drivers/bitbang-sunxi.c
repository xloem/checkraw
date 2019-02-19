#include <checkraw/drivers/bitbang-sunxi.h>
#include <checkraw/errors.h>

#define main __dummy_bitbang_sunxi_main
#include "../../submodules/sunxi-tools/pio.c"
#undef main

#define PIO_PINS_PER_PORT 32
#define PIO_PIN_CT (PIO_PINS_PER_PORT * PIO_NR_PORTS)

checkraw_error bitbang_sunxi_init(bitbang_t * dev)
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

	dev->pin_ct = PIO_PIN_CT;
	dev->features = BF_PULL_FLOAT | BF_PULL_UP | BF_PULL_DOWN;
	dev->handle = buf;
	return CRE_SUCCESS;
}

void bitbang_sunxi_destroy(bitbang_t * dev)
{
	int pagesize = sysconf(_SC_PAGESIZE);
	int offset = 0x01c20800 & (pagesize-1);
	char * buf = dev->handle;
	buf -= offset;
	munmap(dev->handle, (0x800 + pagesize - 1) & ~(pagesize-1));
	dev->handle = NULL;
}

inline static checkraw_error pin2portpin(bitbang_t * dev, unsigned pin, unsigned * port, unsigned * portpin)
{
	if (dev == NULL || dev->handle == NULL) 
	{
		return cr_error(CRE_INVALID_STATE, "sunxi gpio use", "device handle is NULL", dev != NULL);
	}
	if (pin > PIO_PIN_CT)
	{
		return cr_error(CRE_INVALID_ARG, "sunxi gpio use", "invalid pin", pin);
	}
	*port = pin / PIO_PINS_PER_PORT;
	*portpin = pin % PIO_PINS_PER_PORT;
	return CRE_SUCCESS;
}

checkraw_error bitbang_sunxi_mode(bitbang_t * dev, unsigned pin, int is_input, bitbang_feature_t pull)
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

int bitbang_sunxi_read(bitbang_t * dev, unsigned pin)
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

checkraw_error bitbang_sunxi_write(bitbang_t * dev, unsigned pin, int isHigh)
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
