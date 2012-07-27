#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"

#include "ParticleSystem.h"

class SpriteSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	SpriteSystem(const unsigned int particles, const int threads = 0);
	virtual ~SpriteSystem();
	
	/** Cinder update callback */
	virtual void update();
	
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	struct PointSprite {
		ci::Vec2f position;
		ci::Vec2f velocity;
		float mass, alpha;
		
		void init(const float x = 0, const float y = 0, const float u = 0, const float v = 0, const float m = 1, const float a = 1)
		{
			position.x = x;
			position.y = y;
			velocity.x = u;
			velocity.y = v;
			mass = m;
			alpha = a;
		}
	};
	
	struct Emitter {
		
		void setPosition(const ci::Vec2f& pos) { mPosition = pos; }	
		
		void setDirection(const ci::Vec2f& dir) { mDirection = dir; }
		
		ci::Vec2f getPosition() const { return mPosition; }
		
		ci::Vec2f getDirection() const { return mDirection; }
		
		void update(const SpriteSystem& system)
		{
			this->addParticles(system, ci::Vec2f(0,0), ci::Vec2f(0,0));
		}
		
	  protected:
		void addParticles(const SpriteSystem& system, const ci::Vec2f &pos, const ci::Vec2f &vel)
		{
			for(unsigned int i=0; i<mParticleRate; i++){
				system.mParticles[mCurrentIndex].init(pos.x, pos.y, vel.x, vel.y);
				mCurrentIndex++;
				if(mCurrentIndex >= system.mMaxParticles){
					mCurrentIndex = 0;
				}
			}
		}
		
		ci::Vec2f mPosition;
		ci::Vec2f mDirection;
		unsigned int mEmitterRate;
		unsigned int mCurrentIndex;
		unsigned int mParticleRate;
		
		friend class SpriteSystem;
	};
	
protected:
	ci::gl::Texture* mPointTexture;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	PointSprite* mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	Emitter*	mEmitter;

};




