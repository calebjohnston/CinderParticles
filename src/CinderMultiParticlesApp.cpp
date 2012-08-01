#include "cinder/app/AppBasic.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Xml.h"

#include "CinderMultiParticlesApp.h"

using namespace ci;
using namespace ci::app;

CinderMultiParticlesApp::CinderMultiParticlesApp()
 :	mShader(NULL), mBlurX(NULL), mBlurY(NULL), mFade(NULL),
	mEnableGaussianBlur(false), mEnableFade(false), mMouseDown(false),
	mEmitter(NULL), mLineSystem(NULL), mSpriteSystem(NULL)
{}

CinderMultiParticlesApp::~CinderMultiParticlesApp()
{
}

void CinderMultiParticlesApp::setup()
{
	this->setWindowSize(1680,1080);
	this->setFrameRate(60);
	this->setWindowPos(0, 60);
	
	fs::path path = this->getAppPath() / "Contents" / "Resources";
	this->addAssetDirectory(path);
	
//	mParticleSystem = new ParticleSystem();
//	mLineSystem = new LineSystem(50000);
	mSpriteSystem = new SpriteSystem("../Resources/images/particle-small.png", 50000);
	pMouse = getWindowCenter();
	mEmitter = new Emitter(100, pMouse, Vec2f(0,-10.0f));
	
	try {
		mShader = new gl::GlslProg( app::loadAsset( "../Resources/shaders/pass.vert" ), app::loadAsset( "../Resources/shaders/blur.frag" ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		console() << "Shader compile error: " << std::endl;
		console() << exc.what();
	}
	catch( ... ) {
		console() << "Unable to load shader" << std::endl;
	}
	
	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::enableAdditiveBlending();
}

void CinderMultiParticlesApp::update()
{
//	mParticleSystem->computeRandomVectors(15.5f, 5.5f);
	
	mEmitter->setPosition(pMouse);
	
//	mLineSystem->update();
	mSpriteSystem->emit(*mEmitter);
	mSpriteSystem->update();
}

void CinderMultiParticlesApp::draw()
{
	gl::clear();
	gl::color(1,1,1,1);
//	mLineSystem->draw();
	mSpriteSystem->draw();
	
	/*
	if(mEnableFade){
		// first
		gl::clear();
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		mFade->bindTexture(GL_TEXTURE_2D, 0);
		gl::color(0.91f,0.80f,0.88f,1.0f);
		gl::draw(mFade->getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
		mFade->unbindTexture();
		gl::color( ColorA::white() );
		mParticleSystem->draw();
		
		Area inverted = mFade->getBounds();
		inverted.setY1(gl::getViewport().getY2());
		inverted.setY2(gl::getViewport().getY1());
		mFade->blitFromScreen(mFade->getBounds(), inverted, GL_NEAREST, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else if(!mEnableGaussianBlur){
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		mParticleSystem->draw();
	}
	else{
		// FIRST render pass...
		//============================================================
		mBlurX->bindFramebuffer();
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
		mParticleSystem->draw();
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
	}
*/
}

void CinderMultiParticlesApp::resize( ResizeEvent event )
{
	int w = event.getWidth();
	int h = event.getHeight();
	
	// clean data
	if(mBlurX != NULL) delete mBlurX;
	if(mBlurY != NULL) delete mBlurY;
	if(mFade != NULL) delete mFade;
	
	// create FBOs at new window size
	gl::Fbo::Format format;
	format.setColorInternalFormat( GL_RGBA );
	format.enableDepthBuffer(true);
	format.enableMipmapping();
	format.setWrap(GL_CLAMP,GL_CLAMP);
	format.setMinFilter(GL_LINEAR);
	format.setMagFilter(GL_LINEAR);
	//format.setSamples(4);
	mBlurX = new gl::Fbo(w, h, format);
	mBlurY = new gl::Fbo(w, h, format);
	mFade = new gl::Fbo(w, h, format);
	
	mFade->bindFramebuffer();
	gl::clear();
	mFade->unbindFramebuffer();
}

void CinderMultiParticlesApp::keyDown( KeyEvent event )
{ 
    switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
			break;
		case 'd':
			mEnableFade = !mEnableFade;
			break;
		case 'b':
			mEnableGaussianBlur = !mEnableGaussianBlur;
			break;
		case 's':
			// app::writeImage( getHomeDirectory() / "cinder" / "particles" / ("snapshot-" + toString( this->getElapsedFrames() ) + ".png" ), copyWindowSurface() );
			break;
    }
}

void CinderMultiParticlesApp::mouseMove( MouseEvent event )
{
	pMouse = event.getPos();
}

void CinderMultiParticlesApp::mouseDown( MouseEvent event )
{
	mMouseDown = true;
}

void CinderMultiParticlesApp::mouseUp( MouseEvent event )
{
	mMouseDown = false;
}

void CinderMultiParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() );
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) * 0.25f;
	pMouse = event.getPos();
//	mParticleSystem->addParticles(mouseNorm, mouseVel);
}

CINDER_APP_BASIC( CinderMultiParticlesApp, RendererGl )
