#pragma once
#ifndef RGBLIGHTPATTERN_H
#define RGBLIGHTPATTERN_H 1

#include "helpers.h"

using namespace std;

class RgbLightPattern
{
protected:
	const float defaultColor[NUM_COLORS] = { 0.0f };
	const float * baseColor = defaultColor;
public:
	unsigned int refreshInterval = 20; //ms
	RgbLightPattern(const float baseColor[NUM_COLORS]);
	virtual ~RgbLightPattern()=0;
	virtual const float * getColor ()=0;
	virtual RgbLightPattern* clone() const = 0;
};

#endif // !RGBLIGHTPATTERN_H
