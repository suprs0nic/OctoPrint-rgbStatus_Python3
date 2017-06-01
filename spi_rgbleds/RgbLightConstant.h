#pragma once

#ifndef RGBLIGHTCONSTANT_H
#define RGBLIGHTCONSTANT_H 1

#include <vector>
#include "RgbLightPattern.h"

using namespace std;

class RgbLightConstant : public RgbLightPattern {
public:
	RgbLightConstant(vector<float> baseColor) : RgbLightPattern(baseColor) { this->refreshInterval = 200; };
	vector<float> getColor();
	RgbLightPattern* clone() const { return new RgbLightConstant(*this); }
};

#endif // !RGBLIGHTCONSTANT_H

