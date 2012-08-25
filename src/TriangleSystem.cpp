#include "TriangleSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

TriangleSystem::TriangleSystem()
:	ParticleSystem(),
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL)
{
}

TriangleSystem::~TriangleSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mNumCache;
}

void TriangleSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	
	// allocate memory
	try {
		mParticles = (Tri3D*) calloc(sizeof(Tri3D), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 9);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 12);
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

void TriangleSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized) return;
	
	size_t index, vi, ci;
	for(index = start_index; index < end_index; index++){
		Tri3D* line = mParticles + index;
		
		// accumulate system forces
		line->velocity.y += 0.015;
		
		// perform integration
//		line->v1.x = line->v2.x;
//		line->v1.y = line->v2.y;
//		line->v1.z = line->v2.z;
//		line->v2.x = line->v1.x + line->velocity.x;
//		line->v2.y = line->v1.y + line->velocity.y;
//		line->v2.z = line->v1.z + line->velocity.z;
//		line->v3 = cross(line->v1, line->velocity);
		line->v1.x = line->v1.x + line->velocity.x;
		line->v1.y = line->v1.y + line->velocity.y;
		line->v1.z = line->v1.z + line->velocity.z;
		line->v2.x = line->v2.x + line->velocity.x;
		line->v2.y = line->v2.y + line->velocity.y;
		line->v2.z = line->v2.z + line->velocity.z;
		line->v3.x = line->v3.x + line->velocity.x;
		line->v3.y = line->v3.y + line->velocity.y;
		line->v3.z = line->v3.z + line->velocity.z;

		
		// fade out
		line->alpha -= 0.01;
		
		// update the vertex array
		vi = index * 9;
		mPositionArray[vi] = line->v1.x;
		mPositionArray[vi+1] = line->v1.y;
		mPositionArray[vi+2] = line->v1.z;
		mPositionArray[vi+3] = line->v2.x;
		mPositionArray[vi+4] = line->v2.y;
		mPositionArray[vi+5] = line->v2.z;
		mPositionArray[vi+6] = line->v3.x;
		mPositionArray[vi+7] = line->v3.y;
		mPositionArray[vi+8] = line->v3.z;
		
		// update the color array
		ci = index * 12;
		mColorArray[ci] = line->alpha * 0.25f;
		mColorArray[ci+1] = line->alpha * 0.75f;
		mColorArray[ci+2] = 1.0f - (line->alpha * 0.5f);
		mColorArray[ci+3] = line->alpha;
		
		mColorArray[ci+4] = line->alpha * 0.25f;
		mColorArray[ci+5] = line->alpha * 0.75f;;
		mColorArray[ci+6] = 1.0f - (line->alpha * 0.5f);
		mColorArray[ci+7] = line->alpha;
		
		mColorArray[ci+8] = line->alpha * 0.25f;
		mColorArray[ci+9] = line->alpha * 0.75f;;
		mColorArray[ci+10] = 1.0f - (line->alpha * 0.5f);
		mColorArray[ci+11] = line->alpha;
	}
}

void TriangleSystem::emit(const Emitter& emitter)
{	
	if(!mInitialized) return;
	
	// update from emitter
	Vec2f pos = emitter.getPosition();
	Vec2f vel = emitter.getDirection();
	for(unsigned int i=0; i<emitter.getRate(); i++){
		Vec2f p = mNumCache->nextPosition();
		Vec2f v = mNumCache->nextVelocity();
		mParticles[mCurrentIndex++].init(Vec3f(pos.x + p.x, pos.y + p.y, 0.0f), Vec3f(vel.x + v.x, vel.y + v.y, 0.0f));
		if(mCurrentIndex >= mMaxParticles){
			mCurrentIndex = 0;
		}
	}
}

void TriangleSystem::update()
{
	if(!mInitialized) return;
	
	mNumCache->computeRandomVectors(5.0f,10.0f,10.0f);	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void TriangleSystem::draw()
{	
	if(!mInitialized) return;
	
	ParticleSystem::preDraw();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, mPositionArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 0, mColorArray);
	
	glDrawArrays(GL_TRIANGLES, 0, this->mMaxParticles * 3);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	ParticleSystem::postDraw();
}
