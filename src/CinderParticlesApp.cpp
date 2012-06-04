#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"

#include "CinderParticlesApp.h"

void CinderParticlesApp::setup()
{
	mFPS = 60.0f;
	setFrameRate( mFPS );
	
	mParticleSystem = new ParticleSystem();

	this->setWindowSize(1680,1080);
	
	mShader = new gl::GlslProg( loadResource( "../Resources/shaders/particle.vert" ), loadResource( "../Resources/shaders/blur.frag" ) );
	
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGBA );
	format.enableDepthBuffer(true);
	format.enableMipmapping();
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_LINEAR);
	format.setMagFilter(GL_LINEAR);
	format.setSamples(4);
	mBlurX = new gl::Fbo(this->getWindowWidth(), this->getWindowHeight(), format);
	mBlurY = new gl::Fbo(this->getWindowWidth(), this->getWindowHeight(), format);
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
//	mParams = new params::InterfaceGl("settings", Vec2i(200,300));
//	mParams->addText( "fps", &mFpsLabel );
	
	pMouse = getWindowCenter();
	
	gl::enableAdditiveBlending();
}

//void CinderParticlesApp::fadeToColor( float r, float g, float b, float speed )
//{
//	glColor4f( r, g, b, speed );
//	gl::drawSolidRect( getWindowBounds() );
//}

// add force and dye to fluid, and create particles
void CinderParticlesApp::addParticles( Vec2f pos, Vec2f vel )
{
	mParticleSystem->addParticles( pos, vel, Rand::randInt(100) );
}

void CinderParticlesApp::update()
{	
	mParticleSystem->update();
}

void CinderParticlesApp::draw()
{
	// FIRST render pass...
	//============================================================
	mBlurX->bindFramebuffer();
	
	gl::clear();
	gl::color( ColorA::white() );
	gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
	mParticleSystem->draw();
//	gl::drawSolidRect(Rectf(100,100,300,300));
	mBlurX->unbindFramebuffer();
	
	// SECOND render pass...
	//============================================================
	mBlurY->bindFramebuffer();
	gl::clear();
	gl::color( ColorA::white() );
	gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
	mBlurX->bindTexture(GL_TEXTURE_2D, 0);
	mShader->bind();
	mShader->uniform("tex0", 0);
	mShader->uniform("sampleOffset", Vec2f(1.0/getWindowWidth(), 0));
	gl::draw(mBlurX->getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
	mBlurX->unbindTexture();
	mShader->unbind();
	mBlurY->unbindFramebuffer();
	
	// THIRD render pass...
	//============================================================
	gl::clear();
	gl::color( ColorA::white() );
	gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
	mBlurY->bindTexture(GL_TEXTURE_2D, 0);
	mShader->bind();
	mShader->uniform("tex0", 0);
	mShader->uniform("sampleOffset", Vec2f(0, 1.0/getWindowHeight()));
	gl::draw(mBlurY->getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
	mBlurY->unbindTexture();
	mShader->unbind();
	
	// fourth pass (sorta)
	//------------------------------------------------------------
//	gl::color( ColorA::white() );
//	gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
//	mBlurX->bindTexture(GL_TEXTURE_2D, 0);
//	gl::draw(mBlurX->getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
//	mBlurX->unbindTexture();
	
//	params::InterfaceGl::draw();
}

void CinderParticlesApp::resize( ResizeEvent event )
{
	int w = event.getWidth();
	int h = event.getHeight();
	mParticleSystem->setWindowSize( Vec2i( w, h ) );
}

void CinderParticlesApp::keyDown( KeyEvent event )
{ 
    switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
    }
}

void CinderParticlesApp::mouseMove( MouseEvent event )
{
	pMouse = event.getPos();
}

void CinderParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() );// / getWindowSize();
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) * 0.25f;// / getWindowSize();
	pMouse = event.getPos();
	this->addParticles(mouseNorm, mouseVel);
}

CINDER_APP_BASIC( CinderParticlesApp, RendererGl )