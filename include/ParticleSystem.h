/*
 *  ParticleSystem.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */
#pragma once

#include "Particle.h"
#include "cinder/Vector.h"

#define MAX_PARTICLES		10000


class ParticleSystem {
public:	
	ParticleSystem();
	~ParticleSystem();
	
	ci::Vec2i	windowSize;
	ci::Vec2f	invWindowSize;
	Particle	particles[MAX_PARTICLES];
    float		posArray[MAX_PARTICLES * 2 * 2];
    float		colArray[MAX_PARTICLES * 3 * 2];
	int			curIndex;
	
	void update();
	void draw();
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel, int count = 10 );
	void addParticle( const ci::Vec2f &pos, const ci::Vec2f &vel );
	void setWindowSize( const ci::Vec2i &winSize );
};




