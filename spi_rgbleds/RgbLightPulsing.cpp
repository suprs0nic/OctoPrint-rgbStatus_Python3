#include "RgbLightPulsing.h"

RgbLightPulsing::RgbLightPulsing(vector<float> baseColor, unsigned int speed) : RgbLightPattern(baseColor) {
	this->speed = speed; // Speed in milliseconds. 
	this->refreshInterval = 20; // 20 ms --> 50 Hz
	this->toColor = this->baseColor;
	this->maxi = speed / this->refreshInterval;

	// Calculate the change in color per step beforehand
	for (int j = 0; j<NUM_COLORS; j++)
		this->deltaColor[j] = (this->toColor[j] - this->fromColor[j]) / (this->maxi / 2.0f);
}

vector<float> RgbLightPulsing::getColor()
{
	if (this->i >= this->maxi)
	{
		this->i = 0;
		this->currentColor = this->fromColor;
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

	return this->currentColor;
}
