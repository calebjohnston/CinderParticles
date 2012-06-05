/*
 *  Particle.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */

#pragma once

#include "cinder/Vector.h"

class Particle {
  public:
	Particle();
    ci::Vec2f	pos;
	ci::Vec2f	vel;
    float		radius;
    float		alpha;
    float		mass;
	
    void init(float x, float y);
    void init(float x, float y, float u, float v);
    void update( const ci::Vec2f &windowSize, const ci::Vec2f &invWindowSize );
	void updateVertexArrays( const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer);
	void updateVertexArraysLines( const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer);
};

