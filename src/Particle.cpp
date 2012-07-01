#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

using namespace ci;

Particle::Particle()
{
	mPosition = Vec2f( 0,0 );
	mVelocity = Vec2f( 0,0 );
    mMass = 1.0;
	mAlpha = 0.0;
}

Particle::~Particle()
{
}

void Particle::init( float x, float y )
{
	mPosition.x = x;
	mPosition.y = y;
	mVelocity.x = mVelocity.y = 0;
	mAlpha  = 1.0;
	mMass = 0;
}

void Particle::init( float x, float y, float u, float v )
{
	mPosition.x = x;
	mPosition.y = y;
	mVelocity.x = u;
	mVelocity.y = v;
	mAlpha  = 1.0;
	mMass = 0;
}

void Particle::update( const Vec2f &windowSize, const Vec2f &invWindowSize )
{
	// apply gravity (or not)
//	mVelocity.y += 0.1;
	
	// integrate
	mPosition.x += mVelocity.x;
	mPosition.y += mVelocity.y;
	
	// fade out...
	mAlpha -= 0.01;
}

void Particle::updatePointsData( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const 
{
	int vi = i * 2;
	posBuffer[vi] = mPosition.x;
	posBuffer[vi+1] = mPosition.y;
	
	int ci = i * 4;
	colBuffer[ci] = mAlpha;
	colBuffer[ci+1] = mAlpha * 0.5f;
	colBuffer[ci+2] = 1.0 - mAlpha;
	colBuffer[ci+3] = mAlpha;
}

void Particle::updateLinesData( const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) const 
{
	int vi = i * 4;
	posBuffer[vi] = mPosition.x - mVelocity.x;
	posBuffer[vi+1] = mPosition.y - mVelocity.y;
	posBuffer[vi+2] = mPosition.x;
	posBuffer[vi+3] = mPosition.y;
	
	int ci = i * 8;
	colBuffer[ci] = mAlpha;
	colBuffer[ci+1] = mAlpha * 0.5f;
	colBuffer[ci+2] = 1.0 - mAlpha;
	colBuffer[ci+3] = mAlpha;
	
	colBuffer[ci+4] = mAlpha;
	colBuffer[ci+5] = mAlpha * 0.5f;
	colBuffer[ci+6] = 1.0 - mAlpha;
	colBuffer[ci+7] = mAlpha;
}
