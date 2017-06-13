#pragma once
#ifndef PWMDRIVER_H
#define PWMDRIVER_H 1

#define PWM_RESOLUTION 4095
#define PWM_BITS_PER_COLOR 12

#include "spi.h"
#include "helpers.h"

using namespace std;

class PwmDriver
{
public:
	PwmDriver();	
	~PwmDriver();
	void setRgbw(const float left[NUM_COLORS] , const float right[NUM_COLORS] );
};

#endif
