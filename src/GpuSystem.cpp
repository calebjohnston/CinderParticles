#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/Rand.h"

#include "GpuSystem.h"

using namespace ci;
using namespace std;

GpuSystem::GpuSystem(const unsigned int particles, const string& sprite_texture) : GpuParticleSystem(particles)
{
	gl::Texture::Format format;
	format.enableMipmapping(true);
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_NEAREST);
	format.setMagFilter(GL_NEAREST);
	mPointTexture = gl::Texture( loadImage( app::loadAsset(sprite_texture) ), format );
}

GpuSystem::~GpuSystem()
{}

void GpuSystem::emit(const Emitter& emitter)
{
}

void GpuSystem::update()
{	
	GpuParticleSystem::update();
}

void GpuSystem::draw()
{	
	GpuParticleSystem::draw();
	
	/*
	//---------------------------------------------------------------
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_2D);
	mPointTexture.bind(1);
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
	glPointSize(6.0);
	//---------------------------------------------------------------
	
	mFBO[mCurrentFBO].bindTexture(0, 0);
	mDisplShader.bind();
	mDisplShader.uniform("displacementMap", 0);
	mDisplShader.uniform("pointTexture", 1);
	
	gl::draw( mVboMesh );
	
    gl::popModelView();
	
	mDisplShader.unbind();
	mFBO[mCurrentFBO].unbindTexture();
	
	//---------------------------------------------------------------
	glDisable(GL_POINT_SPRITE);
	glDisable(GL_BLEND);
	mPointTexture.unbind();
	//---------------------------------------------------------------
	 */
}
