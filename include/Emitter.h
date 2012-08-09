#pragma once

#include "cinder/Vector.h"

class ParticleSystem;

class Emitter {
public:
	Emitter(const unsigned int rate = 10, const ci::Vec2f& pos = ci::Vec2f::zero(), const ci::Vec2f& dir = ci::Vec2f::one());
	
	Emitter& operator=(const Emitter& rhs)
	{
		this->mPosition = rhs.mPosition;
		this->mDirection = rhs.mDirection;
		this->mEmitterRate = rhs.mEmitterRate;
		
		return *this;
	}
	
	Emitter(const Emitter& rhs) { *this = rhs; }
	
	~Emitter();
	
	void setPosition(const ci::Vec2f& pos) { mPosition = pos; }	
	void setDirection(const ci::Vec2f& dir) { mDirection = dir; }
	void setRate(const unsigned int amount) { mEmitterRate = amount; }
	
	ci::Vec2f getPosition() const { return mPosition; }
	ci::Vec2f getDirection() const { return mDirection; }
	unsigned int getRate() const { return mEmitterRate; }
	
protected:	
	ci::Vec2f mPosition;
	ci::Vec2f mDirection;
	unsigned int mEmitterRate;
};