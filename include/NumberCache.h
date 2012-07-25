#pragma once

#include "cinder/Vector.h"
	
class NumberCache {
public:
	NumberCache();
	~NumberCache();
	
	/** Computes a set of random vectors. Intended to be used once per update step */
	void computeRandomVectors(const float pos_var = 15.0f, const float vel_var = 5.0f, const float scale = 1.0f);
	
private:
	ci::Vec2f*	mRandomPos;
	ci::Vec2f*	mRandomVel;
	float*		mRandomFactor;
	unsigned int mRandomSize;
};