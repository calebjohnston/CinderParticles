#pragma once

#include <vector>

#include "cinder/Thread.h"

/**
 * ParticleSystem base class.
 */
class ParticleSystem {
public:	
	// Cstor initializes data
	virtual ParticleSystem(const unsigned int particles, const int threads = 0);
	virtual ~ParticleSystem();
	
	/** Cinder update callback */
	virtual void update() = 0;
	
	/** Cinder draw callback */
	virtual void draw() = 0;
	
	/** Might need to refactor this... hmmmm */
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Might need to refactor this... hmmmm */
	virtual void threaded_update(const unsigned int start_index, const unsigned int end_index, const int id);
	
	/** Accessor method for the total set of particles */
	unsigned int getMaxParticles() const { return mMaxParticles; }
	
	/** Accessor method used to control multi-threaded processing */
	bool running() const { return mRunning; }
	
protected:
	bool mRunning;
	unsigned int	mMaxParticles;
	boost::shared_mutex	mMutex;
	std::vector<std::thread*> mThreads;
	std::vector<bool> mThreadCompleted;	// not sure if there's a better way of doing this yet...
};




