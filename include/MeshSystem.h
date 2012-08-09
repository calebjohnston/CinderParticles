#pragma once

#include "cinder/gl/Texture.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"
#include "cinder/Vector.h"
#include "cinder/Quaternion.h"
#include "cinder/Color.h"

#include "NumberCache.h"
#include "Emitter.h"
#include "ParticleSystem.h"

class MeshSystem : public ParticleSystem {
public:	
	// Cstor initializes data
	MeshSystem(const std::string& filepath);
	virtual ~MeshSystem();
	
	/** Cinder update callback */
	virtual void setup(const unsigned int particles, const int threads = 0);
	
	/** Cinder update callback */
	virtual void update();
	
	/** */
	virtual void updateKernel(const unsigned int start_index, const unsigned int end_index);
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
	struct MeshParticle {
		ci::gl::VboMesh* mesh;
		ci::Vec3f position;
		ci::Vec3f velocity;
		ci::Quatf rotation;
		float rotationalVelocity;
		float rotationAngle;
		ci::Vec3f rotationAxis;
		ci::ColorA color;
		ci::Vec3f tensor;
		ci::Vec3f scale;
		
		void init(ci::gl::VboMesh* mesh, const ci::Vec3f& pos, const ci::Vec3f& vel, 
				  const ci::Vec3f& scale = ci::Vec3f::one(), 
				  const ci::Vec3f& tensor = ci::Vec3f::zero(), 
				  const ci::Vec3f& rot_axis = ci::Vec3f::one(), 
				  const float rot_vel = 0, const float rot_angle = 0, 
				  const ci::ColorA& color = ci::ColorA::white())
		{
			this->mesh = mesh;
			this->position = pos;
			this->velocity = vel;
			this->rotationAxis = rot_axis;
			this->rotationalVelocity = rot_vel;
			this->rotationAngle = rot_angle;
			this->color = color;
			this->scale = scale;
			this->tensor = tensor;
			this->rotation.set(this->rotationAxis, this->rotationAngle);
		}
	};
	
protected:
    float* mPositionArray;
    float* mColorArray;
	
	ci::Vec2i mWindowSize;
	ci::Vec2f mInvWindowSize;
	MeshParticle* mParticles;
	NumberCache* mNumCache;
	ci::TriMesh* mMesh;
	ci::gl::VboMesh* mVBO;

};




