#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"

#include "CinderParticlesApp.h"

using namespace ci;
using namespace ci::app;

void CinderParticlesApp::setup()
{
	this->getWindow()->getSignalResize().connect(std::bind(&CinderParticlesApp::resize, this));
	
	fs::path path = this->getAppPath() / "Contents" / "Resources";
	this->addAssetDirectory(path);
	
	mShader = NULL;
	//mParticleSystem = NULL;
	mBlurX = mBlurY = NULL;
	mEnableGaussianBlur = mMouseDown = false;

	this->setWindowSize(1680,1080);
	this->setFrameRate(60);
	
//	mParticleSystem = new ParticleSystem();
	mLineSystem = new LineSystem();
	//mSpriteSystem = new SpriteSystem();
	
	mLineEmitter = new Emitter( 1000, Vec2f::zero(), Vec2f::one() );
	mLineSystem->setup( 100000 );
	
	try {
		mShader = new gl::GlslProg( app::loadAsset( "shaders/pass.vert" ), app::loadAsset( "shaders/blur.frag" ) );
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
	
	mParams = params::InterfaceGl::create(this->getWindow(), "Parameters", Vec2i(350, 600));
	
	pMouse = getWindowCenter();
	
	gl::enableAdditiveBlending();
}

void CinderParticlesApp::addParticles( Vec2f pos, Vec2f vel )
{
//	mLineSystem->addParticles( pos, vel );
	mLineEmitter->setPosition( pos );
	mLineEmitter->setDirection( vel );
	mLineSystem->emit( *mLineEmitter );
}

void CinderParticlesApp::update()
{	
	mLineSystem->update();
}

void CinderParticlesApp::draw()
{
	if(!mEnableGaussianBlur){
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		mLineSystem->draw();
	}
	else{
		// FIRST render pass...
		//============================================================
		mBlurX->bindFramebuffer();
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
		mLineSystem->draw();
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
}

void CinderParticlesApp::resize()
{
	int w = getWindowWidth();
	int h = getWindowHeight();
//	mParticleSystem->setWindowSize( Vec2i( w, h ) );
	
	// clean data
	if(mBlurX != NULL) delete mBlurX;
	if(mBlurY != NULL) delete mBlurY;
	
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
}

void CinderParticlesApp::keyDown( KeyEvent event )
{
    switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
			break;
		case 'b':
			mEnableGaussianBlur = !mEnableGaussianBlur;
			break;
    }
}

void CinderParticlesApp::mouseMove( MouseEvent event )
{
	pMouse = event.getPos();
}

void CinderParticlesApp::mouseDown( MouseEvent event )
{
	mMouseDown = true;
}

void CinderParticlesApp::mouseUp( MouseEvent event )
{
	mMouseDown = false;
}

void CinderParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() );
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) * 0.25f;
	pMouse = event.getPos();
	this->addParticles(mouseNorm, mouseVel);
}

CINDER_APP_BASIC( CinderParticlesApp, RendererGl )
