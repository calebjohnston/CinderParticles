#include "SpriteSystem.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#define MAX_PARTICLES 500000

using namespace ci;

SpriteSystem::SpriteSystem(const unsigned int particles, const int threads = 0)
 : ParticleSystem(particles, threads)
{
	mParticles = NULL;
	mColorArray = NULL;
	mPositionArray = NULL;
	mPointTexture = NULL;
	mMaxParticles = (unsigned int) MAX_PARTICLES;
	
	// allocate memory
	try {
		mParticles = (Particle*) calloc(sizeof(Particle), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
	} catch(...) {
		std::cout << "Unable to allocate data" << std::endl;
	}
	
	// initialize particle list (prolly not necessary, we're using structs)
	// for(int i=0; i<this->mMaxParticles; i++) {
	// 	mParticles[i].init();
	// }
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// this should be able to be passed in (and added maybe?)
	mEmitter = new Emitter();
	
	// setup NumberCache
}

SpriteSystem::~SpriteSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mPointTexture;
}

void SpriteSystem::update()
{	
	//this->computeRandomVectors();	// update number cache
	
	// update from emitter
	mEmitter->update(*this);

	// Update kernel(*this)
	/*
	for(int i=0; i<this->mMaxParticles; i++) {
		if(mParticles[i].alpha() > 0) {
			mParticles[i].update(mWindowSize, mInvWindowSize);
			mParticles[i].updatePointsData(mInvWindowSize, i, mPositionArray, mColorArray);
		}
	}
	*/
}

void SpriteSystem::draw()
{	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mPointTexture->getId());
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointSize(12.0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, mPositionArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, mColorArray);
	
	glDrawArrays(GL_POINTS, 0, this->mMaxParticles);
	
	glDisable(GL_POINT_SPRITE);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void Emitter::update(const SpriteSystem& system)
{
	Vec2f pos();
	Vec2f vel();
	this->addParticles(system, pos, vel);
}

// this needs to be updated (for multithreaded codes ???)
void Emitter::addParticles(const SpriteSystem& system, const Vec2f &pos, const Vec2f &vel)
{
	for(unsigned int i=0; i<mParticleRate; i++){
		system.mParticles[mCurrentIndex].init(pos.x, pos.y, vel.x, vel.y);
		mCurrentIndex++;
		if(mCurrentIndex >= system.mMaxParticles){
			mCurrentIndex = 0;
		}
	}
}

#undef MAX_PARTICLES