#pragma once

#ifndef RGBLIGHTONOFF_H
#define RGBLIGHTONOFF_H 1

using namespace std;

#include "helpers.h"
#include "RgbLightPattern.h"

class RgbLightOnOff : public RgbLightPattern {
	int i = 0;
	int maxi = 0;

	int speed = 1000; // Speed in milliseconds

public:
	RgbLightOnOff(float baseColor[NUM_COLORS], unsigned int speed);
	const float *getColor();
	RgbLightPattern* clone() const { return new RgbLightOnOff(*this); }
	~RgbLightOnOff();
};

#endif // RGBLIGHTONOFF_H
