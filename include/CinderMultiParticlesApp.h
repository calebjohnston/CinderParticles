#pragma once

#include <vector>

#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/params/Params.h"
#include "cinder/Thread.h"
#include "cinder/ArcBall.h"
#include "cinder/Camera.h"

#include "Emitter.h"
#include "LineSystem.h"
#include "SpriteSystem.h"
#include "GpuParticleSystem.h"

/**
 * The CinderParticlesApp is a basic particle engine. Some of the 
 * implementation is based off of Memo's particle code. There are 
 * options provided for rendering lines, points, or point sprites.
 * The rendering uses VBOs for the particle geometry.
 * FBOs can be used for custom post-processing.
 */
class CinderMultiParticlesApp : public ci::app::AppBasic {
public:
	CinderMultiParticlesApp();
	~CinderMultiParticlesApp();
	void	setup();
	void	update();
	void	draw();	
	void	addParticles( ci::Vec2f pos, ci::Vec2f vel );
	void	keyDown( ci::app::KeyEvent event );
	void	mouseMove( ci::app::MouseEvent event );
	void	mouseDrag( ci::app::MouseEvent event );
	void	mouseDown( ci::app::MouseEvent event );
	void	mouseUp( ci::app::MouseEvent event );
	void	resize( ci::app::ResizeEvent event );
	
	ci::CameraPersp		mCam;
	ci::Arcball			mArcball;
	
private:
	ParticleSystem*				mLineSystem;
	ParticleSystem*				mSpriteSystem;
	GpuParticleSystem*			mGpuSystem;
	Emitter*					mEmitter;
	ci::gl::GlslProg*			mShader;
	ci::gl::Fbo*				mBlurX;
	ci::gl::Fbo*				mBlurY;
	ci::gl::Fbo*				mFade;
	ci::gl::Texture*			mFadeTexture;
//	ci::params::InterfaceGl*	mParams;
	std::vector<std::thread*>	mThreads;
	
	
	ci::Vec2i					pMouse;
	bool						mMouseDown;
	bool						mEnableGaussianBlur;
	bool						mEnableFade;
};
