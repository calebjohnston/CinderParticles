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
	
	// allocate memory
	particles = (Particle*) malloc(sizeof(Particle) * this->MAX_PARTICLES);
	int scale_factor = 1;
	if(DRAW_LINES) scale_factor = 2;
    posArray = (float*) malloc(sizeof(float) * this->MAX_PARTICLES * 2 * scale_factor);
	colArray = (float*) malloc(sizeof(float) * this->MAX_PARTICLES * 4 * scale_factor);
	
	// initialize particle list
	for(int i=0; i<this->MAX_PARTICLES; i++) {
		particles[i] = Particle();
	}
	
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
//	unsigned int count_ = 0;
	if(DRAW_LINES){
		for(int i=0; i<this->MAX_PARTICLES; i++) {
			if(particles[i].alpha > 0) {
				particles[i].update( windowSize, invWindowSize );
				particles[i].updateVertexArraysLines( invWindowSize, i, posArray, colArray);
			}
		}	
	}
	else{
		for(int i=0; i<this->MAX_PARTICLES; i++) {
			if(particles[i].alpha > 0) {
//				count_++;
				particles[i].update( windowSize, invWindowSize );
				particles[i].updateVertexArrays( invWindowSize, i, posArray, colArray);
			}
		}
//		if(count_==this->MAX_PARTICLES) std::cout << "maxxed out!!" << std::endl;
	}
}

void ParticleSystem::draw(){
	if(DRAW_LINES){
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable( GL_TEXTURE_2D );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(3.0);
		  
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, posArray);
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colArray);
		
		glDrawArrays(GL_LINES, 0, this->MAX_PARTICLES * 2);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else{
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable( GL_TEXTURE_2D );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
		glEnable(GL_POINT_SMOOTH);
		glPointSize(6.0);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, posArray);
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, colArray);
		
		glDrawArrays(GL_POINTS, 0, this->MAX_PARTICLES);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
}


void ParticleSystem::addParticles( const Vec2f &pos, const Vec2f &vel, int count ){
	for(int i=0; i<count; i++){
		addParticle( pos + Rand::randVec2f() * Rand::randFloat(15.0), vel + Rand::randVec2f() * Rand::randFloat(5.0) );
	}
}


void ParticleSystem::addParticle( const Vec2f &pos, const Vec2f &vel ) {
	particles[curIndex].init( pos.x, pos.y, vel.x, vel.y );
	curIndex++;
	if(curIndex >= this->MAX_PARTICLES){
		curIndex = 0;
	}
}
