#include "SpriteSystem.h"

#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

SpriteSystem::SpriteSystem(const std::string& filepath)
 :	ParticleSystem(), 
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL)
{	
	gl::Texture::Format format;
	format.enableMipmapping(true);
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);
	mPointTexture = gl::Texture( loadImage( app::loadAsset(filepath) ), format );
}

SpriteSystem::~SpriteSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mNumCache;
}

void SpriteSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	
	// allocate memory
	try {
		mParticles = (PointSprite*) calloc(sizeof(PointSprite), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
	} catch(...) {
		app::console() << "Unable to allocate data" << std::endl;
		return;
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// maximum size of 2 kilobytes
	
	mInitialized = true;
}

void SpriteSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized) return;
	
	for(size_t index = start_index; index < end_index; index++){
		PointSprite* point = mParticles + index;
		// accumulate system forces
		point->velocity.y += 0.15;
		
		// perform integration
		point->position.x += point->velocity.x;
		point->position.y += point->velocity.y;
		
		// fade out and update color
		point->color.r = point->color.a * 0.25f;
		point->color.g = point->color.a * 0.85f;
		point->color.b = 1.0 - point->color.a;
		point->color.a -= 0.01;
	}
}

void SpriteSystem::update()
{	
	if(!mInitialized) return;
	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void SpriteSystem::emit(const Emitter& emitter)
{
	if(!mInitialized) return;
	
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
}

void SpriteSystem::draw()
{	
	if(!mInitialized) return;
	
	ParticleSystem::preDraw();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, mPointTexture.getId());
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointSize(12.0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 36, mParticles);
	
	// such a hack....
	char* ptr = (char*) mParticles;
	ptr += 16;
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_FLOAT, 36, ptr);
	
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
