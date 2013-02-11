#include "ParticleSystem.h"
#include "CinderMultiParticlesApp.h"

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Thread.h"

using namespace ci;
using namespace ci::app;

ParticleSystem::ParticleSystem() : mMaxParticles(0), mRunning(false), mCurrentIndex(0), mInitialized(false)
{	
}

ParticleSystem::~ParticleSystem() 
{
	mRunning = false;
	
	for(int i = 0; i < mThreads.size(); ++i){
		mThreads.at(i)->join();
	}
	
	mThreadCompleted.clear();
	mThreads.clear();
}

void ParticleSystem::setup(const unsigned int particles, const int threads)
{
	mMaxParticles = particles;
	mCurrentIndex = 0;
	
	if(threads > 0){
		unsigned int workload_size = mMaxParticles / threads;
		unsigned int start_index, end_index;
		for(int i = 0; i < threads; ++i){
			start_index = i * workload_size;
			end_index = start_index + workload_size;
			// here we store data required for the load balancing between threads
			std::thread* t = new std::thread(boost::bind(&ParticleSystem::spawnThread, this, _1, _2, _3), start_index, end_index, i);
			mThreads.push_back(t);
		}
		
	}
	
	mRunning = true;
}

void ParticleSystem::update()
{
	if(!mRunning) return;
	
	if(mThreads.size() <= 0){
		this->updateKernel(0, mMaxParticles);
	}
}

void ParticleSystem::spawnThread(const unsigned int start_index, const unsigned int end_index, const int id)
{
	std::unique_lock<std::mutex> lock(mMutex);
	mThreadCompleted.push_back(false);
	lock.unlock();
	
	while( mRunning ){
		if(!mThreadCompleted[id]){
			this->updateKernel(start_index, end_index);
			
			mThreadCompleted[id] = true;
		}
	}
}

void ParticleSystem::preDraw()
{	
	bool keep_going = true;
	for(size_t i=0; i<mThreadCompleted.size(); i++) {
		keep_going = keep_going && mThreadCompleted[i];
	}
//	if(!keep_going) return;
}

void ParticleSystem::postDraw()
{	
	for(size_t i=0; i<mThreadCompleted.size(); i++) {
		mThreadCompleted[i] = false;
	}
}