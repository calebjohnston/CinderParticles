#pragma once

#include "cinder/Vector.h"
	
class Emitter {
public:
	Emitter();
	~Emitter();
	
	void setPosition(const ci::Vec2f& pos) { mPosition = pos; }	
	
	void setDirection(const ci::Vec2f& dir) { mDirection = dir; }
	
	ci::Vec2f getPosition() const { return mPosition; }
	
	ci::Vec2f getDirection() const { return mDirection; }
	
	// Not sure exactly how this will be handled yet...
	// void update(const LineSystem& system);
	
private:	
	ci::Vec2f mPosition;
	ci::Vec2f mDirection;
	unsigned int mEmitterRate;
	unsigned int mCurrentIndex;
};