#pragma once

#include "cinder/Vector.h"

#include "NumberCache.h"
#include "Emitter.h"
#include "ParticleSystem.h"

class LineSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	LineSystem();
	virtual ~LineSystem();
	
	/** Cinder update callback */
	virtual void setup(const unsigned int particles, const int threads = 0);
	
	/** Cinder update callback */
	virtual void update();
	
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
	/** */
	virtual void addParticles(const unsigned int amount, const ci::Vec2f &pos, const ci::Vec2f &vel);
	
	struct Line2D {
		ci::Vec2f start;
		ci::Vec2f end;
		ci::Vec2f velocity;
		float mass, alpha;
		
		void init(const float x = 0, const float y = 0, const float u = 0, const float v = 0, const float m = 1, const float a = 1)
		{
			start.x = x;
			start.y = y;
			velocity.x = u;
			velocity.y = v;
			end.x = x + u;
			end.y = y + v;
			mass = m;
			alpha = a;
		}
	};
	
protected:
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Line2D*		mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	NumberCache* mNumCache;
};
