#pragma once

#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class ParticleSystem {
public:	
	// Cstor initializes data
	ParticleSystem();
	~ParticleSystem();
	
	/** Cinder update callback */
	void update();
	
	/** Cinder draw callback */
	void draw();
	
	/** Add new particles at the position, velocity and quantity supplied by the parameters */
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel, unsigned int count );
	
	/** Add new particles at the position and velocity supplied by the parameters. 
		Uses default particle rate for quantity */
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel );
	
	/** Add a single new particle at the position and velocity supplied by the parameters */
	void addParticle( const ci::Vec2f &pos, const ci::Vec2f &vel );
	
	/** Establish the window size for rendering operations */
	void setWindowSize( const ci::Vec2i &winSize );

	/** rendering mode determines how particles are represented */
	enum Rendering { NONE, POINTS, LINES, SPRITES };
	
	/** performs memory allocation based upon drawing mode, also called from Cstor */
	void setMode(Rendering mode);
	
protected:
	ci::gl::Texture* mPointTexture;
	Rendering	mRenderType;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Particle*	mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	int			mMaxParticles;
	int			mParticleRate;
	int			mCurrentIndex;
	
};




