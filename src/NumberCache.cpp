#include "cinder/Rand.h"

#include "NumberCache.h"

using namespace ci;

NumberCache::NumberCache()
{}

NumberCache::~NumberCache()
{}

void NumberCache::computeRandomVectors(const float pos_var, const float vel_var, const float scale)
{
	for(size_t i=0; i<mRandomSize; i++){
		mRandomPos[i] = Rand::randVec2f() * Rand::randFloat(pos_var);
		mRandomVel[i] = Rand::randVec2f() * Rand::randFloat(vel_var);
		mRandomFactor[i] = Rand::randFloat(scale);
	}
}