/*
 *  ParticleSystem.cpp
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */

#include "ParticleSystem.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

using namespace ci;

ParticleSystem::ParticleSystem() 
{
	curIndex = 0;
	
	windowSize = app::App::get()->getWindowSize();
	invWindowSize = Vec2f( 1.0f / windowSize.x, 1.0f / windowSize.y );
}

ParticleSystem::~ParticleSystem() 
{
}

void ParticleSystem::setWindowSize( const Vec2i &winSize )
{
	windowSize = winSize;
	invWindowSize = Vec2f( 1.0f / windowSize.x, 1.0f / windowSize.y );
}

void ParticleSystem::update()
{
	for(int i=0; i<MAX_PARTICLES; i++) {
		if(particles[i].alpha > 0) {
			particles[i].update( windowSize, invWindowSize );
			particles[i].updateVertexArrays( invWindowSize, i, posArray, colArray);
		}
	}
}

void ParticleSystem::draw(){
	glEnable(GL_BLEND);
	glDisable( GL_TEXTURE_2D );
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// normal alpha blending
	glBlendFunc( GL_SRC_ALPHA, GL_ONE );	// additive blending
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(3.0);
	  
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, posArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colArray);
	
	glDrawArrays(GL_LINES, 0, MAX_PARTICLES * 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_BLEND);
}


void ParticleSystem::addParticles( const Vec2f &pos, const Vec2f &vel, int count ){
	for(int i=0; i<count; i++){
		addParticle( pos + Rand::randVec2f() * Rand::randFloat(10.0), vel + Rand::randVec2f() * Rand::randFloat() );
	}
}


void ParticleSystem::addParticle( const Vec2f &pos, const Vec2f &vel ) {
	particles[curIndex].init( pos.x, pos.y, vel.x, vel.y );
	curIndex++;
	if(curIndex >= MAX_PARTICLES){
		curIndex = 0;
	}
}
