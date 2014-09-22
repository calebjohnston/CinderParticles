#pragma once

#include "cinder/Vector.h"

#include "NumberCache.h"
#include "Emitter.h"
#include "ParticleSystem.h"

#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

class KernelSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	KernelSystem();
	virtual ~KernelSystem();
	
	/** Cinder update callback */
	virtual void setup(const std::string& oclKernel, const unsigned int particles,
					   cl_context oclContext, cl_command_queue oclCmdQueue,
					   cl_device_id* oclDeviceIds, cl_int oclDeviceCount );
	
	/** Cinder update callback */
	virtual void update();
	
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
	struct cl_Line2D {
		cl_float2 start;
		cl_float2 end;
		cl_float2 velocity;
		cl_float2 mass_alpha;
		
		void init(const float x = 0, const float y = 0, const float u = 0, const float v = 0, const float m = 1, const float a = 1)
		{
			start = { x, y };
			velocity = { u, v };
			end = { x + u, y + v };
			mass_alpha = { m, a };
		}
	};
	
protected:
	cl_kernel	mKernel;
	cl_program	mProgram;
	cl_mem		mDeviceMemBuffer;
	
	ci::Vec2i	mWindowSize;
	ci::Vec2f	mInvWindowSize;
	cl_Line2D*	mParticles;
    float*		mPositionArray;
    float*		mColorArray;
	NumberCache* mNumCache;
};
