#include "RgbLightOnOff.h"

RgbLightOnOff::RgbLightOnOff(vector<float> baseColor, unsigned int speed) : RgbLightPattern(baseColor) {
	this->speed = speed; // Speed in milliseconds. 
	this->refreshInterval = 200; // 20 ms --> 50 Hz
	this->maxi = speed / this->refreshInterval;
}

vector<float> RgbLightOnOff::getColor()
{
	if (this->i >= this->maxi)
		this->i = 0;

	this->i++;

	if (this->i <= this->maxi / 2)
		return this->baseColor;				// On
	else
		return{ 0.0f, 0.0f, 0.0f, 0.0f };	// Off
}
