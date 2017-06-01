#pragma once
#ifndef RGBLIGHTPATTERN_H
#define RGBLIGHTPATTERN_H 1

#include <vector>

using namespace std;

class RgbLightPattern
{
protected:
	vector<float> baseColor;
public:
	unsigned int refreshInterval = 20; //ms
	RgbLightPattern(vector<float> baseColor);
	~RgbLightPattern();
	virtual vector<float> getColor()=0;
	virtual RgbLightPattern* clone() const = 0;
};

#endif // !RGBLIGHTPATTERN_H
