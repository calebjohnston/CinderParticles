#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"

#include "Particle.h"
#include "ParticleSystem.h"

using namespace ci;
using namespace ci::app;

/**
 * The CinderParticlesApp is a basic particle engine. Some of the 
 * implementation is based off of Memo's particle code. There are 
 * options provided for rendering lines, points, or point sprites.
 * The rendering uses VBOs for the particle geometry.
 * FBOs can be used for custom post-processing.
 */
class CinderParticlesApp : public AppBasic {
public:
	void	setup();
	void	update();
	void	draw();	
	void	addParticles( Vec2f pos, Vec2f vel );
	void	keyDown( KeyEvent event );
	void	mouseMove( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	mouseDown( MouseEvent event );
	void	mouseUp( MouseEvent event );
	void	resize( ResizeEvent event );

private:
	ParticleSystem*				mParticleSystem;
	ci::gl::GlslProg*			mShader;
	ci::gl::Fbo*				mBlurX;
	ci::gl::Fbo*				mBlurY;
	ci::params::InterfaceGl*	mParams;
	
	ci::Vec2i					pMouse;
	bool						mMouseDown;
	bool						mEnableGaussianBlur;
};
