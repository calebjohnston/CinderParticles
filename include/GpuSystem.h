#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ImageIO.h"
#include "cinder/Vector.h"

#include "Emitter.h"
#include "GpuParticleSystem.h"

class GpuSystem : public GpuParticleSystem {
public:
	// Cstor initializes data
	GpuSystem(const unsigned int particles, const std::string& sprite_texture);
	virtual ~GpuSystem();
	
	/** Cinder update callback */
	virtual void update();
	
	/** Cinder draw callback */
	virtual void draw();
	
	/** Complies with emitter interface */
	virtual void emit(const Emitter& emitter);
	
protected:
	ci::gl::Texture		mPointTexture;
};
