#include "cinder/Rand.h"

#include "NumberCache.h"

using namespace ci;

NumberCache::NumberCache(const unsigned int byte_size)
{
	size_t total_size = 2 * sizeof(ci::Vec2f) + sizeof(float);	// very brittle :(
	mComponentSize = (unsigned int) floor(byte_size / total_size);
	
	mRandomPos = (Vec2f*) calloc(sizeof(Vec2f), mComponentSize);
	mRandomVel = (Vec2f*) calloc(sizeof(Vec2f), mComponentSize);
	mRandomFactor = (float*) calloc(sizeof(float), mComponentSize);
}

NumberCache::~NumberCache()
{
	delete[] mRandomPos;
	delete[] mRandomVel;
	delete[] mRandomFactor;
}

Vec2f NumberCache::nextPosition()
{
	mPositionIndex++;
	if(mPositionIndex >= mComponentSize) mPositionIndex = 0;
	return mRandomPos[mPositionIndex];
}

Vec2f NumberCache::nextVelocity()
{
	mVeloctyIndex++;
	if(mVeloctyIndex >= mComponentSize) mVeloctyIndex = 0;
	return mRandomVel[mVeloctyIndex];
}

float NumberCache::nextFloat()
{
	mFactorIndex++;
	if(mFactorIndex >= mComponentSize) mFactorIndex = 0;
	return mRandomFactor[mFactorIndex];
}

void NumberCache::computeRandomVectors(const float pos_var, const float vel_var, const float scale)
{
	for(size_t i=0; i<mComponentSize; i++){
		mRandomPos[i] = Rand::randVec2f() * Rand::randFloat(pos_var);
		mRandomVel[i] = Rand::randVec2f() * Rand::randFloat(vel_var);
		mRandomFactor[i] = Rand::randFloat(scale);
	}
}