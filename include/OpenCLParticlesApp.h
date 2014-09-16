#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"

#include "KernelSystem.h"

/**
 *
 */
class OpenCLParticlesApp : public ci::app::AppBasic {
public:
	void	setup();
	void	update();
	void	draw();	
	void	addParticles( ci::Vec2f pos, ci::Vec2f vel );
	void	keyDown( ci::app::KeyEvent event );
	void	mouseMove( ci::app::MouseEvent event );
	void	mouseDrag( ci::app::MouseEvent event );
	void	mouseDown( ci::app::MouseEvent event );
	void	mouseUp( ci::app::MouseEvent event );
	void	resize();

private:
	KernelSystem*				mKernelSystem;
	Emitter*					mEmitter;
	ci::gl::GlslProg*			mShader;
	ci::gl::Fbo*				mBlurX;
	ci::gl::Fbo*				mBlurY;
	ci::params::InterfaceGlRef	mParams;
	
	ci::Vec2i					pMouse;
	bool						mMouseDown;
	bool						mEnableGaussianBlur;
};
