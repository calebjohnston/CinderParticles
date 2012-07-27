#include "LineSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

LineSystem::LineSystem(const unsigned int particles, const int threads) : ParticleSystem(particles, threads)
{
	mParticles = NULL;
	mColorArray = NULL;
	mPositionArray = NULL;
	
	// allocate memory
	try {
		mParticles = (Line2D*) calloc(sizeof(Line2D), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 8);
	} catch(...) {
		ci::app::console() << "Unable to allocate data" << std::endl;
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

LineSystem::~LineSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
}

void LineSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	// nothing yet...
}

void LineSystem::update()
{	
	//this->computeRandomVectors();	// update number cache
	
	// update from emitter
	mEmitter->update(*this);
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void LineSystem::draw()
{	
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
