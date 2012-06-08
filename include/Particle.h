#pragma once

#include "cinder/Vector.h"

class Particle {
public:
	Particle();
	~Particle();
	
	float alpha() const { return mAlpha; }
	float radius() const { return mRadius; }
	float mass() const { return mMass; }
	ci::Vec2f position() const { return mPosition; }
	ci::Vec2f velocity() const { return mVelocity; }
	
	void init(float x, float y);
    void init(float x, float y, float u, float v);
    void update(const ci::Vec2f &windowSize, const ci::Vec2f &invWindowSize);
	void updateLinesData(const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const;
	void updatePointsData(const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const;
	
protected:
    ci::Vec2f	mPosition;
	ci::Vec2f	mVelocity;
    float		mRadius;
    float		mAlpha;
    float		mMass;

};

