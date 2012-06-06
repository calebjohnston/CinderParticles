/*
 *  ParticleSystem.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */

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
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel, int count = 10 );
	void addParticle( const ci::Vec2f &pos, const ci::Vec2f &vel );
	void setWindowSize( const ci::Vec2i &winSize );

	enum Rendering { POINTS, LINES, BILLBOARDS };
	
protected:	
	ci::gl::Texture* mPointTexture;
	int			mMaxParticles;
	Rendering	mRenderType;
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	Particle*	mParticles;
    float*		mPositionArray;
    float*		mColorArray;
    float*		mTexcoordsArray;
	int			mCurrentIndex;
	
};




