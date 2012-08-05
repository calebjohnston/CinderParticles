#include <math.h>

#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/Rand.h"

#include "CinderMultiParticlesApp.h"
#include "GpuSystem.h"

using namespace ci;
using namespace std;

GpuParticleSystem::GpuParticleSystem(const unsigned int particles) : mCurrentIndex(0)
{	
	mTextureSide = (unsigned int) floor(sqrt(particles));
	mMaxParticles = mTextureSide * mTextureSide;
	
	// TODO: verify that we can create textures with the texture size specified...
}

GpuParticleSystem::~GpuParticleSystem()
{}

void GpuParticleSystem::setup(const std::string& update_vertex_shader, const std::string& update_frag_shader, 
							  const std::string& display_vertex_shader, const std::string& display_fragment_shader)
{
	gl::clear();
	try {
		// Multiple render targets shader updates the positions/velocities
		mPosShader = gl::GlslProg( app::loadAsset(update_vertex_shader), app::loadAsset(update_frag_shader));
		// Vertex displacement shader
		mDisplShader = gl::GlslProg( app::loadAsset(display_vertex_shader), app::loadAsset(display_fragment_shader));
	}
	catch( gl::GlslProgCompileExc &exc ) {
		std::cout << "Shader compile error: " << endl;
		std::cout << exc.what();
	}
	catch( ... ) {
		std::cout << "Unable to load shader" << endl;
	}
	
	setupTextures();
	gl::Fbo::Format format;
	format.enableDepthBuffer(false);
	format.enableColorBuffer(true, 2);
	format.setMinFilter( GL_NEAREST );
	format.setMagFilter( GL_NEAREST );
	format.setColorInternalFormat( GL_RGBA32F_ARB );
	mFBO[0] = gl::Fbo( mTextureSide, mTextureSide, format );
	mFBO[1] = gl::Fbo( mTextureSide, mTextureSide, format );
	resetFBOs();
	// THE VBO HAS TO BE DRAWN AFTER FBO!
	setupVBO();
	
	gl::enableDepthRead();
}

void GpuParticleSystem::emit(const Emitter& emitter)
{
	
}

void GpuParticleSystem::addParticles(const unsigned int amount, const ci::Vec2f &pos, const ci::Vec2f &vel)
{
	
}

void GpuParticleSystem::update()
{	
	gl::setMatricesWindow( mFBO[0].getSize(), false ); // false to prevent vertical flipping
	gl::setViewport( mFBO[0].getBounds() );
	
	mFBO[ mCurrentFBO ].bindFramebuffer();
	
	GLenum buf[2] = {GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT};
	glDrawBuffers(2, buf);
	mFBO[ mOtherFBO ].bindTexture(0, 0);
	mFBO[ mOtherFBO ].bindTexture(1, 1);
	mPosShader.bind();
	mPosShader.uniform( "posArray", 0 );
	mPosShader.uniform( "velArray", 1 );
	
	glBegin(GL_QUADS);
	glTexCoord2f( 0.0f, 0.0f); glVertex2f( 0.0f, 0.0f);
	glTexCoord2f( 0.0f, 1.0f); glVertex2f( 0.0f, mTextureSide);
	glTexCoord2f( 1.0f, 1.0f); glVertex2f( mTextureSide, mTextureSide);
	glTexCoord2f( 1.0f, 0.0f); glVertex2f( mTextureSide, 0.0f);
	glEnd();
	
	mPosShader.unbind();
	mFBO[ mOtherFBO ].unbindTexture();
	mFBO[ mCurrentFBO ].unbindFramebuffer();
	
	mCurrentFBO = ( mCurrentFBO + 1 ) % 2;
	mOtherFBO   = ( mCurrentFBO + 1 ) % 2;
}

void GpuParticleSystem::draw()
{	
	CinderMultiParticlesApp* app = (CinderMultiParticlesApp*) app::App::get();
	gl::setMatrices( app->mCam );
	gl::setViewport( app::App::get()->getWindowBounds() );
	gl::clear( ColorA( 0.0f, 0.0f, 0.0f, 1.0f ) );
	
	//---------------------------------------------------------------
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glEnable(GL_TEXTURE_2D);
	//---------------------------------------------------------------
	
	mFBO[mCurrentFBO].bindTexture(0, 0);
	mDisplShader.bind();
	mDisplShader.uniform("displacementMap", 0);
	
	gl::pushModelView();
	gl::translate( Vec3f( 0.0f, 0.0f, app::App::get()->getWindowHeight() / 2.0f ) );
	gl::rotate( app->mArcball.getQuat() );
	
	gl::draw( mVboMesh );
	
    gl::popModelView();
	
	mDisplShader.unbind();
	mFBO[mCurrentFBO].unbindTexture();
	
	//---------------------------------------------------------------
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_TEXTURE_2D);
	//---------------------------------------------------------------

	gl::setMatricesWindow(app::App::get()->getWindowSize());
}

void GpuParticleSystem::setupTextures(){
	// Position 2D texture array
	mInitPos = Surface32f( mTextureSide, mTextureSide, true);
	Surface32f::Iter pixelIter = mInitPos.getIter();
	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			/* Initial particle positions are passed in as R,G,B 
			 float values. Alpha is used as particle mass. */
			mInitPos.setPixel( pixelIter.getPos(), ColorAf( Rand::randFloat()-0.5f, Rand::randFloat()-0.5f, Rand::randFloat()-0.5f, Rand::randFloat(0.2f, 1.0f) ) );
		}
	}
	gl::Texture::Format tFormat;
	tFormat.setInternalFormat(GL_RGBA32F_ARB);
	mPositions = gl::Texture( mInitPos, tFormat);
	mPositions.setWrap( GL_REPEAT, GL_REPEAT );
	mPositions.setMinFilter( GL_NEAREST );
	mPositions.setMagFilter( GL_NEAREST );
	
	//Velocity 2D texture array
	mInitVel = Surface32f( mTextureSide, mTextureSide, true);
	pixelIter = mInitVel.getIter();
	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			/* Initial particle velocities are
			 passed in as R,G,B float values. */
			mInitVel.setPixel( pixelIter.getPos(), ColorAf( 0.0f, 0.0f, 0.0f, 1.0f ) );
		}
	}
	mVelocities = gl::Texture( mInitVel, tFormat);
	mVelocities.setWrap( GL_REPEAT, GL_REPEAT );
	mVelocities.setMinFilter( GL_NEAREST );
	mVelocities.setMagFilter( GL_NEAREST );
}

void GpuParticleSystem::resetFBOs(){
	mCurrentFBO = 0;
	mOtherFBO = 1;
	mFBO[0].bindFramebuffer();
	mFBO[1].bindFramebuffer();
	
	// Attachment 0 - Positions
	glDrawBuffer(GL_COLOR_ATTACHMENT0_EXT);
	gl::setMatricesWindow( mFBO[0].getSize(), false );
	gl::setViewport( mFBO[0].getBounds() );
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mPositions.enableAndBind();
	gl::draw( mPositions, mFBO[0].getBounds() );
	mPositions.unbind();
	
	// Attachment 1 - Velocities
	glDrawBuffer(GL_COLOR_ATTACHMENT1_EXT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mVelocities.enableAndBind();
	gl::draw( mVelocities, mFBO[0].getBounds() );
	mVelocities.unbind();
	
	mFBO[1].unbindFramebuffer();
	mFBO[0].unbindFramebuffer();
	mPositions.disable();
	mVelocities.disable();
}

void GpuParticleSystem::setupVBO()
{
	/*	A dummy VboMesh the same size as the
	 texture to keep the vertices on the GPU */
	vector<Vec2f> texCoords;
	vector<uint32_t> indices;
	gl::VboMesh::Layout layout;
	layout.setStaticIndices();
	layout.setStaticPositions();
	layout.setStaticTexCoords2d();
	layout.setStaticNormals();
	//layout.setDynamicColorsRGBA();
	mVboMesh = gl::VboMesh( mTextureSide, mTextureSide, layout, GL_POINTS);
	for( int x = 0; x < mTextureSide; ++x ) {
		for( int y = 0; y < mTextureSide; ++y ) {	
			indices.push_back( x * mTextureSide + y );
			Vec2f st( x/(float)mTextureSide, y/(float)mTextureSide );
			texCoords.push_back( st );
		}
	}
	mVboMesh.bufferIndices( indices );
	mVboMesh.bufferTexCoords2d( 0, texCoords );
}