#include "LineSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

LineSystem::LineSystem()
:	ParticleSystem(),
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL)
{
}

LineSystem::~LineSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mNumCache;
}

void LineSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	
	// allocate memory
	try {
		mParticles = (Line2D*) calloc(sizeof(Line2D), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 8);
	} catch(...) {
		ci::app::console() << "Unable to allocate data" << std::endl;
		return;
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// use no more than 2 kbytes
}

void LineSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized) return;
	
	size_t index, vi, ci;
	for(index = start_index; index < end_index; index++){
		// accumulate system forces
		mParticles[index].velocity.y += 0.15;
		
		// perform integration
		mParticles[index].start.x = mParticles[index].end.x;
		mParticles[index].start.y = mParticles[index].end.y;
		mParticles[index].end.x = mParticles[index].start.x + mParticles[index].velocity.x;
		mParticles[index].end.y = mParticles[index].start.y + mParticles[index].velocity.y;
		
		// fade out
		mParticles[index].alpha -= 0.01;
		
		// update the vertex array
		vi = index * 4;
		mPositionArray[vi] = mParticles[index].start.x;
		mPositionArray[vi+1] = mParticles[index].start.y;
		mPositionArray[vi+2] = mParticles[index].end.x;
		mPositionArray[vi+3] = mParticles[index].end.y;
		
		// update the color array
		ci = index * 8;
		mColorArray[ci] = mParticles[index].alpha;
		mColorArray[ci+1] = mParticles[index].alpha * 0.5f;
		mColorArray[ci+2] = 1.0 - mParticles[index].alpha;
		mColorArray[ci+3] = mParticles[index].alpha;
		
		mColorArray[ci+4] = mParticles[index].alpha;
		mColorArray[ci+5] = mParticles[index].alpha * 0.5f;
		mColorArray[ci+6] = 1.0 - mParticles[index].alpha;
		mColorArray[ci+7] = mParticles[index].alpha;
	}
	/*
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
		mColorArray[ci] = line->alpha;
		mColorArray[ci+1] = line->alpha * 0.5f;
		mColorArray[ci+2] = 1.0 - line->alpha;
		mColorArray[ci+3] = line->alpha;
		
		mColorArray[ci+4] = line->alpha;
		mColorArray[ci+5] = line->alpha * 0.5f;
		mColorArray[ci+6] = 1.0 - line->alpha;
		mColorArray[ci+7] = line->alpha;
	}
	*/
}

void LineSystem::emit(const Emitter& emitter)
{	
	if(!mInitialized) return;
	
	// update from emitter
	this->addParticles(emitter.getRate(), emitter.getPosition(), emitter.getDirection());
}

void LineSystem::update()
{
	if(!mInitialized) return;
	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void LineSystem::addParticles(const unsigned int amount, const Vec2f &pos, const Vec2f &vel)
{
	if(!mInitialized) return;
	
	for(unsigned int i=0; i<amount; i++){
		Vec2f p = mNumCache->nextPosition();
		Vec2f v = mNumCache->nextVelocity();
		mParticles[mCurrentIndex++].init(pos.x + p.x, pos.y + p.y, vel.x + v.x, vel.y + v.y);
		if(mCurrentIndex >= mMaxParticles){
			mCurrentIndex = 0;
		}
	}
}

void LineSystem::draw()
{	
	if(!mInitialized) return;
	
	ParticleSystem::preDraw();
	
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
	
	ParticleSystem::postDraw();
}
