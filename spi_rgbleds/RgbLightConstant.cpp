#include "RgbLightConstant.h"

RgbLightConstant::RgbLightConstant(const float baseColor[NUM_COLORS]) : RgbLightPattern(baseColor) {
	this->refreshInterval = 20; // 20 ms --> 50 Hz

	// Set colors in colorscheme
	for (int j = 0; j < NUM_COLORS; j++){
		this->currentColor[j] = this->baseColor[j];
	}
}

const float * RgbLightConstant::getColor(){
	return this->currentColor;
}

RgbLightConstant::~RgbLightConstant(){

}
