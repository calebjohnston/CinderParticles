//
//  CinderParticlesApp.h
//  CinderParticles
//
//  Created by Caleb Johnston on 6/4/12.
//

#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/params/Params.h"

#include "Particle.h"
#include "ParticleSystem.h"

using namespace ci;
using namespace ci::app;

class CinderParticlesApp : public AppBasic {
public:
	void	setup();
	
//	void	fadeToColor( float r, float g, float b, float speed );
	void	addParticles( Vec2f pos, Vec2f vel );
	void	keyDown( KeyEvent event );
	void	mouseMove( MouseEvent event );
	void	mouseDrag( MouseEvent event );
	void	resize( ResizeEvent event );
	
	void	update();
	void	draw();
	
	ParticleSystem*			mParticleSystem;
	gl::GlslProg*			mShader;
	gl::Fbo*				mBlurX;
	gl::Fbo*				mBlurY;
	params::InterfaceGl*	mParams;
	
	Vec2i					pMouse;
	float					mFPS;
	std::string				mFpsLabel;
};
