#pragma once

#ifndef RGBLIGHTCONSTANT_H
#define RGBLIGHTCONSTANT_H 1

#include <vector>
#include "RgbLightPattern.h"

using namespace std;

class RgbLightConstant : public RgbLightPattern {
public:
	RgbLightConstant(const float baseColor[NUM_COLORS]) : RgbLightPattern(baseColor) { this->refreshInterval = 200; };
	const float * getColor();
	RgbLightPattern* clone() const { return new RgbLightConstant(*this); }
	~RgbLightConstant();
};

#endif // !RGBLIGHTCONSTANT_H

