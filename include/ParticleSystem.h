/*
 *  ParticleSystem.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */
#pragma once

#include "Particle.h"
#include "cinder/Vector.h"

class ParticleSystem {
public:	
	ParticleSystem();
	~ParticleSystem();
	
	const int	MAX_PARTICLES = 10000;
	const bool	DRAW_LINES = false;
	ci::Vec2i	windowSize;
	ci::Vec2f	invWindowSize;
	Particle*	particles;
    float*		posArray;
    float*		colArray;
	int			curIndex;
	
	void update();
	void draw();
	void addParticles( const ci::Vec2f &pos, const ci::Vec2f &vel, int count = 10 );
	void addParticle( const ci::Vec2f &pos, const ci::Vec2f &vel );
	void setWindowSize( const ci::Vec2i &winSize );
};




