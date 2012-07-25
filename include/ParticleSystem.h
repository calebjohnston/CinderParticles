#pragma once

class ParticleSystem {
public:	
	// Cstor initializes data
	virtual ParticleSystem(const unsigned int particles, const int threads = 0)
	 : mMaxParticles(particles), mThreadcount(threads) {};
	virtual ~ParticleSystem() {};
	
	/** Cinder update callback */
	virtual void update() = 0;
	
	/** Cinder draw callback */
	virtual void draw() = 0;
	
	/** Accessor method for the total set of particles */
	unsigned int getMaxParticles() const { return mMaxParticles; }
	
protected:
	unsigned int	mMaxParticles;
	int				mThreadcount;
};




