#pragma once

#include "cinder/Vector.h"

#include "NumberCache.h"
#include "Emitter.h"
#include "ParticleSystem.h"

class TriangleSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	TriangleSystem();
	virtual ~TriangleSystem();
	
	/** Cinder update callback */
	virtual void setup(const unsigned int particles, const int threads = 0);
	
	/** Cinder update callback */
	virtual void update();
	
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
	struct Tri3D {
		ci::Vec3f v1,v2,v3;
		ci::Vec3f velocity;
		float mass, alpha;
		
		void init(const ci::Vec3f& pos, const ci::Vec3f& vel = ci::Vec3f::zero(), const float m = 1, const float a = 1)
		{
			v1 = pos;
			v2 = pos + ci::Vec3f(5.0f,0,0);
			v3 = pos + ci::Vec3f(0,5.0f,0);
			velocity = vel;
			mass = m;
			alpha = a;
		}
	};
	
protected:
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Tri3D*		mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	NumberCache* mNumCache;
};
