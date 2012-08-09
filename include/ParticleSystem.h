#pragma once

#include <vector>

#include "cinder/Thread.h"

#include "Emitter.h"

/**
 * ParticleSystem base class.
 */
class ParticleSystem {
public:	
	// Cstor initializes data
	ParticleSystem();
	virtual ~ParticleSystem();
	
	/** Cinder update callback */
	virtual void setup(const unsigned int particles, const int threads = 0);
	
	/** Cinder update callback */
	virtual void update();
	
	/** Cinder draw callback */
	virtual void draw() = 0;
	
	/** Might need to refactor this... hmmmm */
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index) = 0;
	
	/** Might need to refactor this... hmmmm */
	virtual void spawnThread(const unsigned int start_index, const unsigned int end_index, const int id);
	
	/** Emitter interface */
	virtual void emit(const Emitter& emitter) = 0;
	
	/** methods to test the status of the threads */
	void preDraw();
	void postDraw();
	
	/** Accessor method for the total set of particles */
	unsigned int getMaxParticles() const { return mMaxParticles; }
	
	/** Accessor method used to control multi-threaded processing */
	bool running() const { return mRunning; }
	
protected:
	bool mRunning;
	bool mInitialized;
	unsigned int	mMaxParticles;
	unsigned int	mCurrentIndex;
	boost::shared_mutex	mMutex;
	std::vector<std::thread*> mThreads;
	std::vector<bool> mThreadCompleted;	// not sure if there's a better way of doing this yet...
};




