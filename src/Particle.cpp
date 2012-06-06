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
	mPosition = Vec2f( 0,0 );
	mVelocity = Vec2f( 0,0 );
    mRadius = mMass = 1.0;
	mAlpha = 0.0;
}

Particle::~Particle()
{
}

void Particle::init( float x, float y ) {
	mPosition.x = x;
	mPosition.y = y;
	mVelocity.x = mVelocity.y = 0;
	mRadius = 20.0;
	mAlpha  = 1.0;
	mMass = 0;
}

void Particle::init( float x, float y, float u, float v ) {
	mPosition.x = x;
	mPosition.y = y;
	mVelocity.x = u;
	mVelocity.y = v;
	mRadius = 20.0;
	mAlpha  = 1.0;
	mMass = 0;
}

void Particle::update( const Vec2f &windowSize, const Vec2f &invWindowSize ) {
	// only update if particle is visible
//	if( alpha == 0 ) return;
	
//	mVelocity.y += 0.1;	// gravity
//	vel.x += 1.0;
	
	mPosition.x += mVelocity.x;
	mPosition.y += mVelocity.y;
/*
	// bounce off edges
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
	mAlpha -= 0.005;
//	if( mAlpha < 0.01f ) mAlpha = 0;
}

void Particle::updatePointsData( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const 
{
	int vi = i * 2;
	posBuffer[vi] = mPosition.x;
	posBuffer[vi+1] = mPosition.y;
	
	int ci = i * 4;
//	if( false ) {
//		colBuffer[ci] = mAlpha;
//		colBuffer[ci+1] = mAlpha;
//		colBuffer[ci+2] = mAlpha;
//		colBuffer[ci+3] = mAlpha;
//	}
//	else {
//		float alphaPer = (1.0 - mAlpha) / 1.01;
//		float r = 1.0f - alphaPer;
//		float g = r * 0.75f;
//		float b = 1.0f - r;
		
		colBuffer[ci] = mAlpha;
		colBuffer[ci+1] = mAlpha * 0.5f;
		colBuffer[ci+2] = 1.0 - mAlpha;
		colBuffer[ci+3] = mAlpha;
//	}
}

void Particle::updateBillboardsData( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer, float* texcoordBuffer) const 
{
	int vi = i * 8;
	posBuffer[vi++] = mPosition.x - mRadius;
	posBuffer[vi++] = mPosition.y - mRadius;
	posBuffer[vi++] = mPosition.x + mRadius;
	posBuffer[vi++] = mPosition.y - mRadius;
	posBuffer[vi++] = mPosition.x + mRadius;
	posBuffer[vi++] = mPosition.y + mRadius;
	posBuffer[vi++] = mPosition.x - mRadius;
	posBuffer[vi++] = mPosition.y + mRadius;
	
	int ti = i * 8;
	texcoordBuffer[ti++] = 0;
	texcoordBuffer[ti++] = 0;
	texcoordBuffer[ti++] = 1.0;
	texcoordBuffer[ti++] = 0;
	texcoordBuffer[ti++] = 1.0;
	texcoordBuffer[ti++] = 1.0;
	texcoordBuffer[ti++] = 0;
	texcoordBuffer[ti++] = 1.0;
	
	int ci = i * 16;
	float alphaPer = (1.0 - mAlpha) / 1.01;
	float r = 1.0f - alphaPer;
	float g = r * 0.75f;
	float b = 1.0f - r;
	
	colBuffer[ci++] = r;
	colBuffer[ci++] = g;
	colBuffer[ci++] = b;
	colBuffer[ci++] = mAlpha;
	
	colBuffer[ci++] = r;
	colBuffer[ci++] = g;
	colBuffer[ci++] = b;
	colBuffer[ci++] = mAlpha;
	
	colBuffer[ci++] = r;
	colBuffer[ci++] = g;
	colBuffer[ci++] = b;
	colBuffer[ci++] = mAlpha;
	
	colBuffer[ci++] = r;
	colBuffer[ci++] = g;
	colBuffer[ci++] = b;
	colBuffer[ci++] = mAlpha;
}


void Particle::updateLinesData( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const 
{
	int vi = i * 4;
	posBuffer[vi++] = mPosition.x - mVelocity.x;
	posBuffer[vi++] = mPosition.y - mVelocity.y;
	posBuffer[vi++] = mPosition.x;
	posBuffer[vi++] = mPosition.y;
	
	int ci = i * 8;
	if( false ) {
		// if drawing fluid, draw lines as black & white
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = mAlpha;
	}
	else {
		float alphaPer = (1.0 - mAlpha) / 1.01;
		float r = 1.0f - alphaPer;
		float g = r * 0.55f;
		float b = 1.0f - r;

		colBuffer[ci++] = r;
		colBuffer[ci++] = g;
		colBuffer[ci++] = b;
		colBuffer[ci++] = mAlpha;
		colBuffer[ci++] = r;
		colBuffer[ci++] = g;
		colBuffer[ci++] = b;
		colBuffer[ci++] = mAlpha;
	}
}
