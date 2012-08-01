#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/Vector.h"

#include "Emitter.h"
#include "ParticleSystem.h"

class SpriteSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	SpriteSystem(const std::string& filepath, const unsigned int particles, const int threads = 0);
	virtual ~SpriteSystem();
	
	/** Cinder update callback */
	virtual void update();
	
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
	/** */
	void addParticles(const unsigned int amount, const ci::Vec2f &pos, const ci::Vec2f &vel);
	
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
	
protected:
	ci::gl::Texture mPointTexture;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	PointSprite* mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	Emitter*	mEmitter;

};




