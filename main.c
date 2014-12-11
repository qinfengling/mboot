/*
 * MM Upgrade program for avalon4 
 * 
 * Upgrade MM firmware through i2c 
 *
 * By Mikeqin, 2014
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "hexdump.c"

#define I2C_DEV		"/dev/i2c-1"
#define I2C_SLAVE_ADDR	1

static int g_i2cfd;

static int i2c_open(char *dev)
{
	if ((g_i2cfd = open(dev, O_RDWR)) < 0) {
		printf("Failed to open i2c port\n");
		return 1;
	}
	return 0;
}

static int i2c_close()
{
	close(g_i2cfd);
	g_i2cfd = (int)NULL;
	return 0;
}

static int i2c_setslave(uint8_t addr)
{
	if (ioctl(g_i2cfd, I2C_SLAVE, addr >> 1) < 0) {
		printf("Unable to find slave addr\n");
		return 1;
	}
	return 0;
}

static int i2c_write(unsigned char *wbuf, unsigned int wlen)
{
	int ret;

	ret = write(g_i2cfd, wbuf, wlen);
	if (ret != wlen) {
		printf("Write to slave failed!(%d-%d)\n", ret, wlen);
		if (ret < 0)
			return -1;

		return 1;
	}

	return 0;
}

static int i2c_read(unsigned char *rbuf, unsigned int rlen)
{
	int ret;

	ret = read(g_i2cfd, rbuf, rlen);
	if (ret != rlen) {
		printf("Read from slave failed!(%d-%d)\n", ret, rlen);
		if (ret < 0)
			return -1;
		return 1;
	}

	return 0;
}

static void sleep_ms(unsigned int ms)
{
    usleep(ms * 1000);
}

#include "mboot.c"

int main(int argc, char **argv)
{
	unsigned char mmpkg[40];

	i2c_open(I2C_DEV);
	i2c_setslave(I2C_SLAVE_ADDR);

#ifdef MM_TEST
	/* MM DETECT PACKAGE */
	memset(mmpkg, 0, 40);
	mmpkg[0] = 0x43;
	mmpkg[1] = 0x4e;
	mmpkg[2] = 0x10;
	mmpkg[3] = 0x00;
	mmpkg[4] = 0x01;
	mmpkg[5] = 0x01;

	printf("Detect MM:\n");
	hexdump(mmpkg, 40);

	/* Wait mm ack */
	sleep_ms(20);

	if (!i2c_read(mmpkg, 40)) {
		printf("Ack MM:\n");
		hexdump(mmpkg, 40);
	}
#else
	mboot();
#endif
	i2c_close();

	return 0;
}

