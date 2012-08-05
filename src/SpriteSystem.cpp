#include "SpriteSystem.h"

#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

SpriteSystem::SpriteSystem(const std::string& filepath, const unsigned int particles, const int threads)
 :	ParticleSystem(particles, threads), 
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL)
{	
	// allocate memory
	try {
		mParticles = (PointSprite*) calloc(sizeof(PointSprite), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4);
	} catch(...) {
		app::console() << "Unable to allocate data" << std::endl;
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	gl::Texture::Format format;
	format.enableMipmapping(true);
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);
	mPointTexture = gl::Texture( loadImage( app::loadAsset(filepath) ), format );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// maximum size of 2 kilobytes
}

SpriteSystem::~SpriteSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mNumCache;
}

void SpriteSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	for(size_t index = start_index; index < end_index; index++){
		PointSprite* point = mParticles + index;
		// accumulate system forces
		point->velocity.y += 0.15;
		
		// perform integration
		point->position.x += point->velocity.x;
		point->position.y += point->velocity.y;
		
		// fade out and update color
		point->color.r = point->color.a;
		point->color.g = point->color.a * 0.5f;
		point->color.b = 1.0 - point->color.a;
		point->color.a -= 0.01;
	}
}

void SpriteSystem::update()
{	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void SpriteSystem::emit(const Emitter& emitter)
{
	this->addParticles(emitter.getRate(), emitter.getPosition(), emitter.getDirection());
}

void SpriteSystem::addParticles(const unsigned int amount, const Vec2f &pos, const Vec2f &vel)
{
	for(unsigned int i=0; i<amount; i++){
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
