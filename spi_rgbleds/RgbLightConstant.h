#pragma once

#ifndef RGBLIGHTCONSTANT_H
#define RGBLIGHTCONSTANT_H 1

#include <algorithm>

using namespace std;

#include "helpers.h"
#include "RgbLightPattern.h"


class RgbLightConstant : public RgbLightPattern {
	float currentColor[NUM_COLORS] = { 0.0f };
public:
	RgbLightConstant(const float baseColor[NUM_COLORS]);
	const float * getColor();
	RgbLightPattern* clone() const { return new RgbLightConstant(*this); }
	~RgbLightConstant();
};

#endif // !RGBLIGHTCONSTANT_H

