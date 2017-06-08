#pragma once

#ifndef SPI_H
#define SPI_H 1

#include <stdlib.h>
#include <stdio.h>

#ifdef __linux__
#include <fcntl.h>				//Needed for SPI port
#include <sys/ioctl.h>			//Needed for SPI port
#include <linux/spi/spidev.h>	//Needed for SPI port
#include <unistd.h>				//Needed for SPI port
#include <string>
#include <iostream>
#include <cstring>

#include "bcm2835.h"
#define CS_PIN		RPI_V2_GPIO_P1_24	// The chip select pin for SPI0
#define SPI_DEVICE "/dev/spidev0.0"

static int spi_cs_fd;
static unsigned char spi_mode;
static unsigned char spi_bitsPerWord;
static unsigned int spi_speed;

static struct timespec tim;
tim.tv_sec = 0;
tim.tv_nsec = 1000; // 1 us

#endif

int spiOpen();
int spiClose();
int spiWrite(unsigned char *data, int length);

#endif // !SPI_H
