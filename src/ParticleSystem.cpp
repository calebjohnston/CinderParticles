/*
 *  ParticleSystem.cpp
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *
 */

#include "ParticleSystem.h"

#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

#define MAX_PARTICLES 160000

using namespace ci;

ParticleSystem::ParticleSystem() 
{
	mPointTexture = NULL;
	mCurrentIndex = 0;
	mRenderType = POINTS;
	mMaxParticles = MAX_PARTICLES;
	
	// load texture -- PARAMETERIZE!
	if(mRenderType == BILLBOARDS){
		mPointTexture = new gl::Texture( loadImage( app::App::get()->loadResource("../Resources/images/particle.png") ) );
	}
	
	// allocate memory
	mParticles = (Particle*) calloc(sizeof(Particle), this->mMaxParticles);
	int scale_factor = 1;
	if(mRenderType == LINES) scale_factor = 2;
	else if(mRenderType == BILLBOARDS) scale_factor = 4;
    mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2 * scale_factor);
	mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4 * scale_factor);
	if(mRenderType == BILLBOARDS){
		mTexcoordsArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2 * scale_factor);
	}else{
		mTexcoordsArray = NULL;
	}
	
	// initialize particle list
	for(int i=0; i<this->mMaxParticles; i++) {
		mParticles[i] = Particle();
	}
	
	setWindowSize(app::App::get()->getWindowSize());
}

ParticleSystem::~ParticleSystem() 
{
	delete mPointTexture;
	delete mParticles;
	delete mPositionArray;
	delete mColorArray;
	delete mTexcoordsArray;
}

void ParticleSystem::setWindowSize( const Vec2i &winSize )
{
	mWindowSize = winSize;
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
}

void ParticleSystem::update()
{
//	unsigned int count_ = 0;
	if(mRenderType == LINES){
		for(int i=0; i<this->mMaxParticles; i++) {
			if(mParticles[i].alpha() > 0) {
				mParticles[i].update(mWindowSize, mInvWindowSize);
				mParticles[i].updateLinesData(mInvWindowSize, i, mPositionArray, mColorArray);
			}
		}	
	}
	else if(mRenderType == BILLBOARDS){
		for(int i=0; i<this->mMaxParticles; i++) {
//			if(mParticles[i].alpha() > 0) {
				mParticles[i].update(mWindowSize, mInvWindowSize);
				mParticles[i].updateBillboardsData(mInvWindowSize, i, mPositionArray, mColorArray, mTexcoordsArray);
//			}
		}	
	}
	else{
		for(int i=0; i<this->mMaxParticles; i++) {
			if(mParticles[i].alpha() > 0) {
//				count_++;
				mParticles[i].update(mWindowSize, mInvWindowSize);
				mParticles[i].updatePointsData(mInvWindowSize, i, mPositionArray, mColorArray);
			}
		}
//		if(count_==this->mMaxParticles) std::cout << "maxxed out!!" << std::endl;
	}
}

void ParticleSystem::draw()
{
	if(mRenderType == LINES){
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_LINE_SMOOTH);
		glLineWidth(4.0);
		  
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, mPositionArray);
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, mColorArray);
		
		glDrawArrays(GL_LINES, 0, this->mMaxParticles * 2);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else if(mRenderType == BILLBOARDS){
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		
		glEnable(GL_TEXTURE_2D);
		mPointTexture->bind(0);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, mPositionArray);
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, mColorArray);
		
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, 0, mTexcoordsArray);
		
		glDrawArrays(GL_QUADS, 0, this->mMaxParticles * 4);
		
		mPointTexture->unbind();
		glDisable(GL_TEXTURE_2D);
		
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
	}
	else {
		glEnable(GL_BLEND);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(6.0);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(2, GL_FLOAT, 0, mPositionArray);
		
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_FLOAT, 0, mColorArray);
		
		glDrawArrays(GL_POINTS, 0, this->mMaxParticles);
		
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
	mParticles[mCurrentIndex].init( pos.x, pos.y, vel.x, vel.y );
	mCurrentIndex++;
	if(mCurrentIndex >= this->mMaxParticles){
		mCurrentIndex = 0;
	}
}
