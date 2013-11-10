#include "ParticleSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

#define MAX_PARTICLES 150000

using namespace ci;

ParticleSystem::ParticleSystem() 
{
	mParticles = NULL;
	mColorArray = NULL;
	mPositionArray = NULL;
	mPointTexture = NULL;
	mCurrentIndex = 0;
	mMaxParticles = 0;
	mParticleRate = 0;
	mRenderType = NONE;	// mode starts unset
	
	// initialize data and set mode
	this->setMode(LINES);
	
	setWindowSize(app::App::get()->getWindowSize());
}

void ParticleSystem::setMode(Rendering mode)
{
	if(mode == NONE) mode = POINTS;
	if(mode == mRenderType) return;
	
	mRenderType = mode;
	
	// load texture -- PARAMETERIZE!
	if(mRenderType == SPRITES){
		if(mPointTexture != NULL) delete mPointTexture;
		
		gl::Texture::Format format;
		format.enableMipmapping(true);
		format.setWrap(GL_CLAMP,GL_CLAMP);
		format.setMinFilter(GL_NEAREST);
		format.setMagFilter(GL_NEAREST);
		mPointTexture = new gl::Texture( loadImage( app::App::get()->loadResource("../Resources/images/particle-small.png") ), format );
	}
	
	// determine memory requirements
	int scale_factor = 1;
	if(mRenderType == LINES) scale_factor = 2;
	
	// scale maximal values based upon processing requirements
	switch(mRenderType){
		case POINTS:
			mMaxParticles = MAX_PARTICLES;
			mParticleRate = 750;
			break;
		case LINES:
			mMaxParticles = (int) MAX_PARTICLES / 2;
			mParticleRate = 600;
			break;
		case SPRITES:
			mMaxParticles = (int) MAX_PARTICLES / 4;
			mParticleRate = 200;
			break;
		default:
		case NONE:
			mMaxParticles = 0;
			mParticleRate = 0;
			break;
	}
	
	// allocate memory
	try {
		mParticles = (Particle*) calloc(sizeof(Particle), this->mMaxParticles);
		mPositionArray = (float*) calloc(sizeof(float), this->mMaxParticles * 2 * scale_factor);
		mColorArray = (float*) calloc(sizeof(float), this->mMaxParticles * 4 * scale_factor);
	} catch(...) {
		std::cout << "Unable to allocate data" << std::endl;
	}
	
	// initialize particle list
	for(int i=0; i<this->mMaxParticles; i++) {
		mParticles[i] = Particle();
	}
	
	// populate random number lists
	mParticleRandomPos = (ci::Vec2f*) malloc(sizeof(ci::Vec2f) * mParticleRate);
	mParticleRandomVel = (ci::Vec2f*) malloc(sizeof(ci::Vec2f) * mParticleRate);
	mParticleRandomFactor = (float*) malloc(sizeof(float) * mParticleRate);
	
	this->computeRandomVectors(15.0f, 5.0f, 2.5f);
}

ParticleSystem::~ParticleSystem() 
{
	delete mParticles;
	delete mColorArray;
	delete mPositionArray;
	delete mPointTexture;
}

void ParticleSystem::setWindowSize( const Vec2i &winSize )
{
	mWindowSize = winSize;
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
}

void ParticleSystem::threaded_update(const unsigned int start_index, const unsigned int end_index, const int id)
{
	std::unique_lock<std::mutex> lock(mMutex);
	std::cout << "id = " << id << ", start_index = " << start_index << ", end_index = " << end_index << std::endl;
	mThreadCompleted.push_back(false);
	lock.unlock();
	
	CinderMultiParticlesApp* application = (CinderMultiParticlesApp*) app::App::get();
	while( application->running() ){
		if(!mThreadCompleted[id]){
			if(mRenderType == LINES){
				for(int i=start_index; i<end_index; i++) {
					if(mParticles[i].alpha() > 0) {
						mParticles[i].update(mWindowSize, mInvWindowSize);
						mParticles[i].updateLinesData(mInvWindowSize, i, mPositionArray, mColorArray);
					}
				}	
			}
			else{
				for(int i=start_index; i<end_index; i++) {
					if(mParticles[i].alpha() > 0) {
						mParticles[i].update(mWindowSize, mInvWindowSize);
						mParticles[i].updatePointsData(mInvWindowSize, i, mPositionArray, mColorArray);
					}
				}
			}
			
			mThreadCompleted[id] = true;
		}
	}
}

void ParticleSystem::computeRandomVectors(const float pos_var, const float vel_var, const float scale)
{
	for(unsigned int i=0; i<mParticleRate; i++){
		mParticleRandomPos[i] = Rand::randVec2f() * Rand::randFloat(pos_var);
		mParticleRandomVel[i] = Rand::randVec2f() * Rand::randFloat(vel_var);
		mParticleRandomFactor[i] = Rand::randFloat(scale);
	}
}

void ParticleSystem::update()
{	
	this->computeRandomVectors();
	
	if(mRenderType == LINES){
		for(int i=0; i<this->mMaxParticles; i++) {
			if(mParticles[i].alpha() > 0) {
				mParticles[i].update(mWindowSize, mInvWindowSize);
				mParticles[i].updateLinesData(mInvWindowSize, i, mPositionArray, mColorArray);
			}
		}	
	}
	else{
		for(int i=0; i<this->mMaxParticles; i++) {
			if(mParticles[i].alpha() > 0) {
				mParticles[i].update(mWindowSize, mInvWindowSize);
				mParticles[i].updatePointsData(mInvWindowSize, i, mPositionArray, mColorArray);
			}
		}
	}
	
	for(int i=0; i<mThreadCompleted.size(); i++) {
		mThreadCompleted[i] = true;
	}
}

void ParticleSystem::draw()
{	
	bool keep_going = true;
	for(int i=0; i<mThreadCompleted.size(); i++) {
		keep_going = keep_going && mThreadCompleted[i];
	}
	if(!keep_going) return;
	
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
	else if(mRenderType == SPRITES){
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
	
	for(int i=0; i<mThreadCompleted.size(); i++) {
		mThreadCompleted[i] = false;
	}
}

void ParticleSystem::addParticles( const Vec2f &pos, const Vec2f &vel, unsigned int count )
{
	for(unsigned int i=0; i<count; i++){
		addParticle( pos + mParticleRandomPos[i], vel + mParticleRandomVel[i]);
	}
}

void ParticleSystem::addParticles( const Vec2f &pos, const Vec2f &vel )
{
	for(unsigned int i=0; i<mParticleRate; i++){
		addParticle( pos + mParticleRandomPos[i], vel + mParticleRandomVel[i]);
	}
}

void ParticleSystem::addParticle( const Vec2f &pos, const Vec2f &vel )
{
	mParticles[mCurrentIndex].init( pos.x, pos.y, vel.x, vel.y );
	mCurrentIndex++;
	if(mCurrentIndex >= this->mMaxParticles){
		mCurrentIndex = 0;
	}
}
