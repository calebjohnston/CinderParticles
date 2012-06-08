#pragma once

#include "Particle.h"
#include "cinder/Vector.h"
#include "cinder/gl/Texture.h"

class ParticleSystem {
public:	
	ParticleSystem();
	~ParticleSystem();
	
	void update();
	void draw();
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel, unsigned int count );
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel );
	void addParticle( const ci::Vec2f &pos, const ci::Vec2f &vel );
	void setWindowSize( const ci::Vec2i &winSize );

	enum Rendering { NONE, POINTS, LINES, BILLBOARDS };
	
	void setMode(Rendering mode);
	
protected:	
	ci::gl::Texture* mPointTexture;
	Rendering	mRenderType;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Particle*	mParticles;
    float*		mPositionArray;
    float*		mColorArray;
    float*		mTexcoordsArray;
	int			mMaxParticles;
	int			mParticleRate;
	int			mCurrentIndex;
	
};




