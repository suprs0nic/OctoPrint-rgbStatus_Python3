#pragma once
#ifndef PWMDRIVER_H
#define PWMDRIVER_H 1

#define PWM_RESOLUTION 4095
#define PWM_BITS_PER_COLOR 12

#include <vector>

#include "spi.h"
#include "helpers.h"

using namespace std;

class PwmDriver
{
public:
	PwmDriver();	
	~PwmDriver();
	void setRgbw(vector<float> left, vector<float> right);
};

#endif
