/*
 *  Particle.cpp
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */

#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

using namespace ci;

Particle::Particle()
{
	pos = Vec2f( 0,0 );
	vel = Vec2f( 0,0 );
    radius = alpha = mass = 1.0;
}

void Particle::init( float x, float y ) {
	pos.x = x;
	pos.y = y;
	vel.x = vel.y = Rand::randFloat( 0.3f, 1 );
	radius = 5;
	alpha  = 1.0;//Rand::randFloat( 0.3f, 1 );
	mass = Rand::randFloat( 0.1f, 1 );
}

void Particle::init( float x, float y, float u, float v ) {
	pos.x = x;
	pos.y = y;
	vel.x = u;
	vel.y = v;
	radius = 5;
	alpha  = 1.0;//Rand::randFloat( 0.3f, 1 );
	mass = Rand::randFloat( 0.1f, 1 );
}

void Particle::update( const Vec2f &windowSize, const Vec2f &invWindowSize ) {
	// only update if particle is visible
	if( alpha == 0 )
		return;
	
	pos += vel;

	// bounce off edges
	/*
	if( pos.x < 0 ) {
		pos.x = 0;
		vel.x *= -1;
	}
	else if( pos.x > windowSize.x ) {
		pos.x = windowSize.x;
		vel.x *= -1;
	}
	
	if( pos.y < 0 ) {
		pos.y = 0;
		vel.y *= -1;
	}
	else if( pos.y > windowSize.y ) {
		pos.y = windowSize.y;
		vel.y *= -1;
	}
	*/
	
	// fade out a bit (and kill if alpha == 0);
	alpha *= 0.99f;
	if( alpha < 0.01f )
		alpha = 0;
}

void Particle::updateVertexArrays( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) {
	int vi = i * 2;
	posBuffer[vi++] = pos.x;
	posBuffer[vi++] = pos.y;
	
	int ci = i * 4;
	if( false ) {
		// if drawing fluid, draw lines as black & white
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
	}
	else {
		float alphaPer = (1.0 - alpha) / 1.01;
		float r = 1.0f - alphaPer;
		float g = r * 0.75f;
		float b = 1.0f - r;
		
		colBuffer[ci++] = r;
		colBuffer[ci++] = g;
		colBuffer[ci++] = b;
		colBuffer[ci++] = alpha;
	}
}


void Particle::updateVertexArraysLines( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) {
	int vi = i * 4;
	posBuffer[vi++] = pos.x - vel.x;
	posBuffer[vi++] = pos.y - vel.y;
	posBuffer[vi++] = pos.x;
	posBuffer[vi++] = pos.y;
	
	int ci = i * 8;
	if( false ) {
		// if drawing fluid, draw lines as black & white
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
	}
	else {
		float alphaPer = (1.0 - alpha) / 1.01;
		float r = 1.0f - alphaPer;
		float g = r * 0.75f;
		float b = 1.0f - r;

		colBuffer[ci++] = r;
		colBuffer[ci++] = g;
		colBuffer[ci++] = b;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = r;
		colBuffer[ci++] = g;
		colBuffer[ci++] = b;
		colBuffer[ci++] = alpha;
	}
}
