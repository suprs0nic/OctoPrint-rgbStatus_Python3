#include "RgbLightConstant.h"

const float * RgbLightConstant::getColor()
{
	for (i = 0; i < NUM_COLORS; i++) {
		this->currentColor[i] = this->baseColor[i];
	}
	return this->currentColor;
}

RgbLightConstant::~RgbLightConstant()
{

}
