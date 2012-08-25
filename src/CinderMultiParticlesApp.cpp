#include "cinder/app/AppBasic.h"
#include "cinder/ObjLoader.h"
#include "cinder/TriMesh.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Xml.h"
#include "cinder/Utilities.h"
#include "cinder/Quaternion.h"

#include "CinderMultiParticlesApp.h"

using namespace ci;
using namespace ci::app;

CinderMultiParticlesApp::CinderMultiParticlesApp()
 :	mShader(NULL), mBlurX(NULL), mBlurY(NULL), mFade(NULL),
	mEnableGaussianBlur(false), mEnableFade(false), mMouseDown(false),
	mEmitter(NULL), mLineSystem(NULL), mSpriteSystem(NULL), mGpuSystem(NULL)
{}

CinderMultiParticlesApp::~CinderMultiParticlesApp()
{
}

void CinderMultiParticlesApp::setup()
{
	this->setWindowSize(1650,1080);
	this->setFrameRate(60);
	this->setWindowPos(0, 60);
	
	fs::path path = this->getAppPath() / "Contents" / "Resources";
	this->addAssetDirectory(path);
	
	TriMesh mMesh;
	//ObjLoader loader((DataSourceRef) loadAsset("meshes/triangle0.obj"));
	ObjLoader loader((DataSourceRef) loadAsset("meshes/head.obj"));
	loader.load( &mMesh );
	mVBO = gl::VboMesh( mMesh );
	
	mLineSystem = new LineSystem();
//	mLineSystem->setup(200000, 2);
	mSpriteSystem = new SpriteSystem("images/particle-small.png");
//	mSpriteSystem->setup(5000);
//	mMeshSystem = new MeshSystem("meshes/Jewel01b.obj");
	mMeshSystem = new MeshSystem("meshes/triangle0.obj");
//	mMeshSystem->setup(5000);
	mTriangleSystem = new TriangleSystem();
	mTriangleSystem->setup(200000, 2);
	
	gl::GlslProg vaoShader;
	try {
		vaoShader = gl::GlslProg( app::loadAsset("shaders/vaomesh.vert"), app::loadAsset( "shaders/vaomesh.frag" ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		console() << "Shader compile error: " << std::endl;
		console() << exc.what();
	}
	catch( ... ) {
		console() << "Unable to load shader" << std::endl;
	}
	
//	mVaoMeshSystem = new VaoMeshSystem("meshes/Jewel02.obj", vaoShader);
//	mVaoMeshSystem->setup(1000);
	mVaoMeshSystem = new VaoMeshSystem("meshes/triangle0.obj", vaoShader);
//	mVaoMeshSystem->setup(5000);
	_rot = 0.0f;
	mContinue = true;
	
//	mGpuSystem = new GpuParticleSystem(262144);
//	mGpuSystem->setup("shaders/pos.vert", "shaders/pos.frag", "shaders/vDispl.vert", "shaders/vDispl.frag");
	
	pMouse = getWindowCenter();
	mEmitter = new Emitter(1000, pMouse, Vec2f(0,-15.0f));
	mEmitter2 = new Emitter(5, pMouse, Vec2f(-0.5f,-10.0f));
	mEmitter3 = new Emitter(50, pMouse, Vec2f(0.9f,-0.5f));
	
	try {
		mShader = new gl::GlslProg( app::loadAsset("shaders/pass.vert"), app::loadAsset( "shaders/blur.frag" ) );
	}
	catch( ci::gl::GlslProgCompileExc &exc ) {
		console() << "Shader compile error: " << std::endl;
		console() << exc.what();
	}
	catch( ... ) {
		console() << "Unable to load shader" << std::endl;
	}
	
//	gl::enableDepthRead();
//	gl::enableDepthWrite();
}

void CinderMultiParticlesApp::update()
{	
	if(!mContinue) return;
	
	mEmitter->setPosition(pMouse);
//	mEmitter2->setPosition(pMouse);
//	mEmitter3->setPosition(pMouse);
//
//	mLineSystem->emit(*mEmitter);
//	mLineSystem->update();
//
//	mSpriteSystem->emit(*mEmitter);
//	mSpriteSystem->update();
//
//	mMeshSystem->emit(*mEmitter2);
//	mMeshSystem->update();
//	mMeshSystem2->emit(*mEmitter3);
//	mMeshSystem2->update();
//	mVaoMeshSystem->update();
	
	mTriangleSystem->emit(*mEmitter);
	mTriangleSystem->update();
	
//	mGpuSystem->update();
}

void CinderMultiParticlesApp::drawSystems()
{
	mTriangleSystem->draw();
//	mLineSystem->draw();
//	mSpriteSystem->draw();
//	mMeshSystem->draw();
//	mMeshSystem2->draw();
	
	
//	glMatrixMode( GL_PROJECTION );
//	glLoadIdentity();
//	glOrtho( 0, getWindowWidth(), getWindowHeight(), 0, 0.1f, 1000.0f );
//	glMatrixMode( GL_MODELVIEW );
//	glLoadIdentity();
//	glViewport( 0, 0, getWindowWidth(), getWindowHeight() );
//	gl::scale(20,20,20);
//	mVaoMeshSystem->draw();
	
//	if(_rot >= 30){
//		console() << " getAverageFps() = " << getAverageFps() << std::endl;
//		_rot = 0;
//	}
//	else {
//		_rot += 1;
//	}
}

void CinderMultiParticlesApp::draw()
{
	if(!mContinue) return;
	
//	gl::setMatrices( mCam );
//	gl::setViewport( getWindowBounds() );
//	gl::clear();
//	
//	mGpuSystem->draw();
//	return;
	
	
	/*
	gl::clear();
//	gl::color(1,1,1,1);
//	gl::pushModelView();
//	gl::translate(200,200,0);
//	gl::scale(20,20,20);
//	glRotatef(_rot, 0.0f,1.0f,0.0f);
//	gl::draw(mVBO);
//	gl::popModelView();

	gl::pushModelView();
//	gl::translate(0,0,-450);
	gl::draw(mVBO);
	gl::popModelView();
	_rot += 1.0f;
	
	if((int)_rot % 30 == 0){
		console() << " getAverageFps() = " << getAverageFps() << std::endl;
	}
	
	return;
	*/
	_rot += 1.0f;
	
	if((int)_rot % 30 == 0){
		console() << " getAverageFps() = " << getAverageFps() << std::endl;
	}
	
//	mContinue = false;
	
	if(mEnableFade){
		// first
		gl::clear();
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		mFade->bindTexture(GL_TEXTURE_2D, 0);
		gl::color(0.91f,0.80f,0.88f,1.0f);
		gl::draw(mFade->getTexture(0), Rectf( 0, 0, getWindowWidth(), getWindowHeight() ) );
		mFade->unbindTexture();
		gl::color(1,1,1,1);
		
		this->drawSystems();
		
		Area inverted = mFade->getBounds();
		inverted.setY1(gl::getViewport().getY2());
		inverted.setY2(gl::getViewport().getY1());
		mFade->blitFromScreen(mFade->getBounds(), inverted, GL_NEAREST, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	else if(!mEnableGaussianBlur){
		gl::clear();
		gl::color(1,1,1,1);
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		
		this->drawSystems();
	}
	else{
		// FIRST render pass...
		//============================================================
		mBlurX->bindFramebuffer();
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
		
		this->drawSystems();
		
		mBlurX->unbindFramebuffer();
		
		// SECOND render pass...
		//============================================================
		mBlurY->bindFramebuffer();
		gl::clear();
		gl::color(1,1,1,1);
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
		gl::color(1,1,1,1);
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
	
	// -- 
	{
		mArcball.setWindowSize( getWindowSize() );
		mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
		mArcball.setRadius( getWindowHeight() / 2.0f );
		
		mCam.lookAt( Vec3f( 0.0f, 0.0f, -450.0f ), Vec3f::zero() );
		mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 2000.0f );
		gl::setMatrices( mCam );
	}
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
		case 'p':
			mContinue = true;
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
	// -- 
	mArcball.mouseDown( event.getPos() );
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
	
	// -- 
	mArcball.mouseDrag( event.getPos() );
}

CINDER_APP_BASIC( CinderMultiParticlesApp, RendererGl )
