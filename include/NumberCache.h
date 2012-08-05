#pragma once

#include "cinder/Vector.h"
	
class NumberCache {
public:
	NumberCache(const unsigned int byte_size = 16384);
	~NumberCache();
	
	/** Accessor methods for obtaining another number component */
	ci::Vec2f nextPosition();
	ci::Vec2f nextVelocity();
	float nextFloat();
	
	/** Computes a set of random vectors. Intended to be used once per update step */
	void computeRandomVectors(const float pos_var = 15.0f, const float vel_var = 5.0f, const float scale = 1.0f);
	
private:
	ci::Vec2f*	mRandomPos;
	ci::Vec2f*	mRandomVel;
	float*		mRandomFactor;
	unsigned int mComponentSize;
	unsigned int mPositionIndex;
	unsigned int mVeloctyIndex;
	unsigned int mFactorIndex;
};