#pragma once

#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Vector.h"

#include "Emitter.h"

/**
 * Shader-based ParticleSystem base class.
 */
class GpuParticleSystem {
public:	
	// Cstor initializes data
	GpuParticleSystem(const unsigned int particles);
	virtual ~GpuParticleSystem();
	
	/** Method used to load required shaders */
	virtual void setup(const std::string& update_vertex_shader, 
					   const std::string& update_frag_shader, 
					   const std::string& display_vertex_shader, 
					   const std::string& display_fragment_shader);
	
	/** Cinder update callback */
	virtual void update();
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Emitter interface */
	virtual void emit(const Emitter& emitter);
	
	/** Accessor method for the total set of particles */
	unsigned int getMaxParticles() const { return mMaxParticles; }
	
protected:	
	unsigned int		mMaxParticles;
	unsigned int		mCurrentIndex;
	
	// -- GPU specific data & methods ------------------------------
	virtual void setupVBO();
	virtual void resetFBOs();
	virtual void setupTextures();
	
	unsigned int		mTextureSide;
	ci::Surface32f		mInitPos;
	ci::Surface32f		mInitVel;
	size_t				mCurrentFBO;
	size_t				mOtherFBO;
	ci::gl::Fbo			mFBO[2];
	ci::gl::Texture		mPositions;
	ci::gl::Texture		mVelocities;
	ci::gl::VboMesh		mVboMesh;
	ci::gl::GlslProg	mPosShader;
	ci::gl::GlslProg	mDisplShader;
};




