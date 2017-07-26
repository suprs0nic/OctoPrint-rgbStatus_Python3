#include "RgbLightPulsing.h"

RgbLightPulsing::RgbLightPulsing(const float baseColor[NUM_COLORS], unsigned int speed) : RgbLightPattern(baseColor) {
	this->speed = speed; // Speed in milliseconds. 
	this->refreshInterval = 20; // 20 ms --> 50 Hz
	
	this->maxi = speed / this->refreshInterval;

	// Calculate the change in color per step beforehand
	for (int j = 0; j < NUM_COLORS; j++) {
		this->deltaColor[j] = (this->baseColor[j] - this->fromColor[j]) / (this->maxi / 2.0f);
		this->currentColor[j] = this->baseColor[j];
	}
}

RgbLightPulsing::~RgbLightPulsing()
{

}

const float * RgbLightPulsing::getColor()
{
/*
	if (this->i >= this->maxi)
	{
		this->i = 0;

		for (int j = 0; j < NUM_COLORS; j++)
			this->currentColor[j] = this->fromColor[j];
	}

	this->i++;

	if (this->i <= this->maxi / 2)
	{
		for (int j = 0; j < NUM_COLORS; j++)
			this->currentColor[j] = min(this->currentColor[j] + this->deltaColor[j], 1.0f);
	}
	else
	{
		for (int j = 0; j < NUM_COLORS; j++)
			this->currentColor[j] = max(this->currentColor[j] - this->deltaColor[j], 0.0f);
	}
*/
	return this->currentColor;
}
