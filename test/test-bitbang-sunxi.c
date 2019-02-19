#include "test-common.h"

/* TODO: this checks against sysfs, but sysfs does not update direction when changed elsewhere, so these checks are commented out.
	recommend instead adding pyA20 as a submodule and checking against its gpio_lib.* sources */

#include <checkraw/bitbang.h>
#include <glob.h>
#include <string.h>

char * pathcat(char * path, char * file)
{
	static char fullpath[1024];

	/* craft full path */
	check(strlen(path) + strlen(file) + 2, <, sizeof(fullpath)); /* no string overflow */
	strcpy(fullpath, path);
	strcat(fullpath, "/");
	strcat(fullpath, file);

	return fullpath;
}

void sysfswrite(char * path, char * file, char * value)
{
	FILE * f;

	/* open and write */
	check(f = fopen(pathcat(path, file), "w"), !=, NULL);
	check(fprintf(f, value), ==, strlen(value));
	fclose(f);
}

char * sysfsread(char * path, char * file)
{
	FILE * f;
	static char buf[1024];

	/* open and read */
	check(f = fopen(pathcat(path, file), "r"), !=, NULL);
	size_t bytes_read = fread(buf, 1, sizeof(buf), f);
	check(bytes_read, <, sizeof(buf) - 1);
	buf[bytes_read] = 0;

	return buf;
}

int main()
{
	bitbang_t * dev = NULL;
	checkraw_error result;
	size_t i;

	/* find device */
	for (i = 0; i < bitbang_nr_drivers(); ++ i)
	{
		if (0 == strcmp(bitbang_driver(i)->name, "sunxi-tools pio"))
		{
			dev = bitbang_driver(i);
			break;
		}
	}

	check(bitbang_init(dev), ==, CRE_SUCCESS);

	/* verify API-set values are reported by sysfs interface */
	/* export pin 1 */
	sysfswrite("/sys/class/gpio", "export", "1");

	/* get path and pin number of sysfs pin 1 */
	glob_t glob_result;
	check(glob("/sys/class/gpio/gpio1_*", 0, NULL, &glob_result), ==, 0);
	check(glob_result.gl_pathc, ==, 1);

	char * gpio_path = glob_result.gl_pathv[0];
	size_t gpio_path_len = strlen(gpio_path);

	/* '(c & 31) - 1' converts a letter to an int in a case-insensitive way */
	unsigned port = (gpio_path[gpio_path_len-2] & 31) - 1;
	unsigned pin = port * 32 + (gpio_path[gpio_path_len-1] - '0');

	/* setup for test */
	sysfswrite(gpio_path, "active_low", "0");

	/* read tests */
	int pin_value;
	check(bitbang_mode(dev, pin, 1, BF_PULL_DOWN), ==, CRE_SUCCESS);
	//check(strcmp(sysfsread(gpio_path, "direction"), "in\n"), ==, 0);
	pin_value = sysfsread(gpio_path, "value")[0] - '0';
	check(bitbang_read(dev, pin), ==, pin_value);
	check(bitbang_mode(dev, pin, 1, BF_PULL_UP), ==, 0);
	//check(strcmp(sysfsread(gpio_path, "direction"), "in\n"), ==, 0);
	pin_value = sysfsread(gpio_path, "value")[0] - '0';
	check(bitbang_read(dev, pin), ==, pin_value);

	/* write tests */
	check(bitbang_mode(dev, pin, 0, BF_PULL_FLOAT), ==, CRE_SUCCESS);
	//check(strcmp(sysfsread(gpio_path, "direction"), "out\n"), ==, 0);
	check(bitbang_write(dev, pin, 0), ==, CRE_SUCCESS);
	check(strcmp(sysfsread(gpio_path, "value"), "0\n"), ==, 0);
	check(bitbang_write(dev, pin, 1), ==, CRE_SUCCESS);
	check(strcmp(sysfsread(gpio_path, "value"), "1\n"), ==, 0);

	/* leave pin in neutral state */
	bitbang_mode(dev, pin, 1, BF_PULL_FLOAT);

	/* cleanup */
	globfree(&glob_result);
	bitbang_destroy(dev);

	return 0;
}
