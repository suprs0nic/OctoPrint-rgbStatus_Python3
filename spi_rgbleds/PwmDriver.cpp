#include "PwmDriver.h"

PwmDriver::PwmDriver()
{
	spiOpen();
}

PwmDriver::~PwmDriver()
{
	spiClose();
}

// Send RGBW color data to the left and right PWM controllers
// We use ports 0-3 for left, 6-9 for right
void PwmDriver::setRgbw(vector<float> left, vector<float> right)
{
	// Find the number of bytes to send (includes space for the empty ports)
	const int nBytes = 2*PWM_BITS_PER_COLOR/8 + PWM_BITS_PER_COLOR * NUM_COLORS * NUM_LEDS / 8;
	unsigned char dataBuffer[nBytes] = { 0 };

	// Only 12 bits per color is currently supported
	if (PWM_BITS_PER_COLOR == 12)
	{
		int j = 0;
		unsigned int b1, b2;

		// Reverse colors 
		for (int i = NUM_COLORS * 2 - 1; i >= 0; i -= 2)
		{
			// First send left, then right
			if (i < NUM_COLORS)
			{
				b1 = (unsigned int) (right[i] * PWM_RESOLUTION);
				b2 = (unsigned int) (right[i-1] * PWM_RESOLUTION);
			}
			else
			{
				b1 = (unsigned int) (left[i - NUM_COLORS] * PWM_RESOLUTION);
				b2 = (unsigned int) (left[i - 1 - NUM_COLORS] * PWM_RESOLUTION);
			}

			// Merge two 12-bit color values into three bytes
			dataBuffer[j] = (b1 & 0xFF0) >> 4;
			dataBuffer[j + 1] = (b1 & 0x00F) << 4 | (b2 & 0xF00) >> 8;
			dataBuffer[j + 2] = (b2 & 0x0FF);

			j += 3;

			// Skip port 4, 5
			if (j == 6)
				j += 3;
		}
	}

	// Send the data over SPI
	spiWrite(dataBuffer, nBytes);
}
