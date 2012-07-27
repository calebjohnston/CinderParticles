#include "SpriteSystem.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

SpriteSystem::SpriteSystem(const unsigned int particles, const int threads)
 : ParticleSystem(particles, threads)
{
	mParticles = NULL;
	mColorArray = NULL;
	mPositionArray = NULL;
	mPointTexture = NULL;
	
	// allocate memory
	try {
		mParticles = (PointSprite*) calloc(sizeof(PointSprite), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
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
	
	gl::Texture::Format format;
	format.enableMipmapping(true);
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);
//	mPointTexture = new gl::Texture( loadImage( app::App::get()->loadAsset("../Resources/images/particle-small.png") ), format );
	
	// setup NumberCache
}

SpriteSystem::~SpriteSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mPointTexture;
}

void SpriteSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	// nothing yet...
}

void SpriteSystem::update()
{	
	//this->computeRandomVectors();	// update number cache
	
	// update from emitter
	mEmitter->update(*this);
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void SpriteSystem::draw()
{	
	ParticleSystem::preDraw();
	
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
	
	ParticleSystem::postDraw();
}
