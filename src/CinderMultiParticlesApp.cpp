#include "cinder/app/AppBasic.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Xml.h"

#include "CinderMultiParticlesApp.h"

using namespace ci;
using namespace ci::app;

CinderMultiParticlesApp::~CinderMultiParticlesApp()
{
	mRunning = false;
	
	for(int i = 0; i < mThreads.size(); ++i){
		mThreads.at(i)->join();
	}
}

void CinderMultiParticlesApp::setup()
{
	mParams = NULL;
	mShader = NULL;
	mParticleSystem = NULL;
	mBlurX = mBlurY = mFade = NULL;
	mEnableGaussianBlur = mEnableFade = mMouseDown = mRunning = false;

	this->setWindowSize(1680,1080);
	this->setFrameRate(60);
	this->setWindowPos(0, 60);
	
	mParticleSystem = new ParticleSystem();
	
	try {
		mShader = new gl::GlslProg( loadResource( "../Resources/shaders/pass.vert" ), loadResource( "../Resources/shaders/blur.frag" ) );
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
	
	// mParams = new params::InterfaceGl("settings", Vec2i(200,300));
	
	pMouse = getWindowCenter();
	
	gl::enableAdditiveBlending();
	int core_count = System::getNumCores() - 1;	// better to stick with 3 threads (+1 for master)
	mRunning = true;
	unsigned int workload_size = mParticleSystem->getMaxParticles() / core_count;
	unsigned int start_index, end_index;
	for(int i = 0; i < core_count; ++i){
		start_index = i * workload_size;
		end_index = start_index + workload_size;
		std::thread* t = new std::thread(boost::bind(&ParticleSystem::threaded_update, mParticleSystem, _1, _2, _3), start_index, end_index, i);
		mThreads.push_back(t);
	}
}

void CinderMultiParticlesApp::update()
{
	mParticleSystem->computeRandomVectors(15.5f, 5.5f);
}

void CinderMultiParticlesApp::draw()
{
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

	if(mParams!=NULL) params::InterfaceGl::draw();
}

void CinderMultiParticlesApp::resize( ResizeEvent event )
{
	int w = event.getWidth();
	int h = event.getHeight();
	mParticleSystem->setWindowSize( Vec2i( w, h ) );
	
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
			writeImage( getHomeDirectory() / "cinder" / "particles" / ("snapshot-" + toString( this->getElapsedFrames() ) + ".png" ), copyWindowSurface() );
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
	mParticleSystem->addParticles(mouseNorm, mouseVel);
}

CINDER_APP_BASIC( CinderMultiParticlesApp, RendererGl )
