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
	vector<float> currentColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	vector<float> fromColor = { 0.0f, 0.0f, 0.0f, 0.0f };
	vector<float> toColor = { 0.0f, 0.0f, 0.0f, 0.0f };

	vector<float> deltaColor = { 0.0f, 0.0f, 0.0f, 0.0f };

public:
	RgbLightPulsing(vector<float> baseColor, unsigned int speed);
	vector<float> getColor();
	RgbLightPattern* clone() const { return new RgbLightPulsing(*this); }

};

#endif // RGBLIGHTPULSING_H
