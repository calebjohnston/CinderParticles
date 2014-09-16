#include "KernelSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

KernelSystem::KernelSystem()
:	ParticleSystem(),
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL)
{
}

KernelSystem::~KernelSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mNumCache;
}

void KernelSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	
	// allocate memory
	try {
//		mParticles = (Line2D*) calloc(sizeof(Line2D), this->mMaxParticles);
//		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
//		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 8);
	} catch(...) {
		ci::app::console() << "Unable to allocate data" << std::endl;
		return;
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// use no more than 2 kbytes
	
	mInitialized = true;
}

void KernelSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized) return;
	/*
	size_t index, vi, ci;
	for(index = start_index; index < end_index; index++){
		Line2D* line = mParticles + index;
		
		// accumulate system forces
		line->velocity.y += 0.15;
		
		// perform integration
		line->start.x = line->end.x;
		line->start.y = line->end.y;
		line->end.x = line->start.x + line->velocity.x;
		line->end.y = line->start.y + line->velocity.y;
		
		// fade out
		line->alpha -= 0.01;
		
		// update the vertex array
		vi = index * 4;
		mPositionArray[vi] = line->start.x;
		mPositionArray[vi+1] = line->start.y;
		mPositionArray[vi+2] = line->end.x;
		mPositionArray[vi+3] = line->end.y;
		
		// update the color array
		ci = index * 8;
		mColorArray[ci] = line->alpha * 0.25f;
		mColorArray[ci+1] = line->alpha * 0.75f;
		mColorArray[ci+2] = 1.0f - (line->alpha * 0.5f);
		mColorArray[ci+3] = line->alpha;
		
		mColorArray[ci+4] = line->alpha * 0.25f;
		mColorArray[ci+5] = line->alpha * 0.75f;;
		mColorArray[ci+6] = 1.0f - (line->alpha * 0.5f);
		mColorArray[ci+7] = line->alpha;
	}
	*/
}

void KernelSystem::emit(const Emitter& emitter)
{	
	if(!mInitialized) return;
	
	// update from emitter
	/*
	Vec2f pos = emitter.getPosition();
	Vec2f vel = emitter.getDirection();
	for(unsigned int i=0; i<emitter.getRate(); i++){
		Vec2f p = mNumCache->nextPosition();
		Vec2f v = mNumCache->nextVelocity();
		mParticles[mCurrentIndex++].init(pos.x + p.x, pos.y + p.y, vel.x + v.x, vel.y + v.y);
		if(mCurrentIndex >= mMaxParticles){
			mCurrentIndex = 0;
		}
	}
	 */
}

void KernelSystem::update()
{
	if(!mInitialized) return;
	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void KernelSystem::draw()
{	
	if(!mInitialized) return;
	
	ParticleSystem::preDraw();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(2.0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, mPositionArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, mColorArray);
	
	glDrawArrays(GL_LINES, 0, this->mMaxParticles * 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	ParticleSystem::postDraw();
}
