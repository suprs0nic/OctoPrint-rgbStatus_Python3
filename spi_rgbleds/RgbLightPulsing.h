#pragma once

#ifndef RGBLIGHTPULSING_H
#define RGBLIGHTPULSING_H 1

#include <algorithm>

using namespace std;

#include "helpers.h"
#include "RgbLightPattern.h"


class RgbLightPulsing : public RgbLightPattern {
	int i = 0;
	int maxi = 0;

	int speed = 1000; // Speed in milliseconds

	float currentColor[NUM_COLORS] = { 0.0f };
	float fromColor[NUM_COLORS] = { 0.0f };
	float deltaColor[NUM_COLORS] = { 0.0f };

public:
	RgbLightPulsing(const float baseColor[NUM_COLORS], unsigned int speed);
	const float * getColor ();
	RgbLightPattern* clone() const { return new RgbLightPulsing(*this); }
	~RgbLightPulsing();

};

#endif // RGBLIGHTPULSING_H
