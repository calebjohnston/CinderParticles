#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"

#include "ParticleSystem.h"

class SpriteSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	virtual SpriteSystem(const unsigned int particles, const int threads = 0) : ParticleSystem(particles, threads);
	virtual ~SpriteSystem();
	
	/** Cinder update callback */
	virtual void update();
	
	/** Cinder draw callback */
	virtual void draw();
	
	struct PointSprite {
		float[2] position;
		float[2] velocity;
		float mass, alpha;
		
		void init(const float x = 0, const float y = 0, const float u = 0, const float v = 0, const float m = 1, const float a = 1)
		{
			position[0] = x;
			position[1] = y;
			velocity[0] = u;
			velocity[1] = v;
			mass = m;
			alpha = a;
		}
	};
	
	struct Emitter {
		
		void setPosition(const ci::Vec2f& pos) { mPosition = pos; }	
		
		void setDirection(const ci::Vec2f& dir) { mDirection = dir; }
		
		ci::Vec2f getPosition() const { return mPosition; }
		
		ci::Vec2f getDirection() const { return mDirection; }
		
		void update(const SpriteSystem& system);
		
	  protected:
		void addParticles(const SpriteSystem& system, const Vec2f &pos, const Vec2f &vel);
		
		ci::Vec2f mPosition;
		ci::Vec2f mDirection;
		unsigned int mEmitterRate;
		unsigned int mCurrentIndex;
		
		friend class SpriteSystem;
	};
	
protected:
	ci::gl::Texture* mPointTexture;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Line2D*		mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	Emitter*	mEmitter;

};




