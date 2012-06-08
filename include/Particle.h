#pragma once

#include "cinder/Vector.h"

class Particle {
public:
	// Cstor initializes data
	Particle();
	~Particle();
	
	// Accessor methods
	float alpha() const { return mAlpha; }
	float mass() const { return mMass; }
	ci::Vec2f position() const { return mPosition; }
	ci::Vec2f velocity() const { return mVelocity; }
	
	/** initializes particle at the point (x,y) */
	void init(float x, float y);
	
	/** initializes particle at the point (x,y) with velocity (u,v) */
    void init(float x, float y, float u, float v);
	
	/** performs particle integration using inputs for scaling values to the display */
    void update(const ci::Vec2f &windowSize, const ci::Vec2f &invWindowSize);
	
	/** updates position and color buffers, one coordinate per particle */
	void updatePointsData(const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const;
	
	/** updates position and color buffers using 2 coordinates per particle */
	void updateLinesData(const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const;
	
protected:
    ci::Vec2f	mPosition;
	ci::Vec2f	mVelocity;
    float		mAlpha;
    float		mMass;

};

