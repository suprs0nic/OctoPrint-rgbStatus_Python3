#include "spi.h"

#ifdef _WIN32

int spiOpen()
{
	printf("SpiOpenPort\n");
	return 0;
}

int spiClose()
{
	printf("SpiClosePort\n");
	return 0;
}

int spiWrite(unsigned char *data, int length)
{
	return 0;
}

#endif

#ifdef __linux__

int spiOpen()
{
	int status_value = -1;

	// SpiDev keeps CS low or high the entire transfer,
	// but we just want a short blip after the transfer.
	// Therefore, we use the bcm2835 GPIO interface, and don't
	// rely on spidev's handling of CS. Unfortunately, we 
	// can't use libbcm2835's SPI functions, as they require
	// (root) access to /dev/mem

	if (!bcm2835_init())
	{
		perror("Could not initialize BCM2835 library");
		exit(1);
	}

	//  Set CE to low by default
	bcm2835_gpio_fsel(CS_PIN, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(CS_PIN, LOW);

	// SPI_MODE_0 (0,0) 	CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge
	// SPI_NO_CS, don't trigger the chip select pin on sending data (we do that with GPIO)
	spi_mode = SPI_MODE_0 | SPI_NO_CS;

	spi_bitsPerWord = 8;
	spi_speed = 30000; // 30kHz, max for TLC5947

	spi_cs_fd = open(SPI_DEVICE, O_RDWR);

	if (spi_cs_fd < 0)
	{
		perror("Error - Could not open SPI device");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MODE, &spi_mode);
	if (status_value < 0)
	{
		perror("Could not set SPIMode (WR)...ioctl fail");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MODE, &spi_mode);
	if (status_value < 0)
	{
		perror("Could not set SPIMode (RD)...ioctl fail");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_WR_BITS_PER_WORD, &spi_bitsPerWord);
	if (status_value < 0)
	{
		perror("Could not set SPI bitsPerWord (WR)...ioctl fail");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_RD_BITS_PER_WORD, &spi_bitsPerWord);
	if (status_value < 0)
	{
		perror("Could not set SPI bitsPerWord(RD)...ioctl fail");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi_speed);
	if (status_value < 0)
	{
		perror("Could not set SPI speed (WR)...ioctl fail");
		exit(1);
	}

	status_value = ioctl(spi_cs_fd, SPI_IOC_RD_MAX_SPEED_HZ, &spi_speed);
	if (status_value < 0)
	{
		perror("Could not set SPI speed (RD)...ioctl fail");
		exit(1);
	}

	return(status_value);
}


int spiClose()
{
	int status_value = -1;
	
	status_value = close(spi_cs_fd);
	if (status_value < 0)
	{
		perror("Error - Could not close SPI device");
		exit(1);
	}

	bcm2835_close();

	return(status_value);
}

int spiWrite(unsigned char *data, int length)
{
	// An array transfers
	struct spi_ioc_transfer spi[length];

	int i = 0;
	int retVal = -1;

	// Fill the transfer array (one transfer equals one byte)
	for (i = 0; i < length; i++)
	{
		memset(&spi[i], 0, sizeof(spi[i]));
		spi[i].tx_buf = (unsigned long)(data + i); // transmit from "data"
		spi[i].rx_buf = (unsigned long)(data + i); // receive into "data"
		spi[i].len = sizeof(*(data + i));
		spi[i].delay_usecs = 0;
		spi[i].speed_hz = spi_speed;
		spi[i].bits_per_word = spi_bitsPerWord;
		spi[i].cs_change = 0;
	}

	// Send a message containing the transfers
	retVal = ioctl(spi_cs_fd, SPI_IOC_MESSAGE(length), &spi);

	if (retVal < 0)
	{
		perror("Error - Problem transmitting spi data..ioctl");
		exit(1);
	}

	// Shortly blip the CS port
	bcm2835_gpio_write(CS_PIN, HIGH);
	nanosleep(1000);
	bcm2835_gpio_write(CS_PIN, LOW);

	return retVal;
}
#endif
