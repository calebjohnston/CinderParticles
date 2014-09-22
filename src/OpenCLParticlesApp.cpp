#include "cinder/app/AppBasic.h"
#include "cinder/Rand.h"

#include "OpenCLParticlesApp.h"

using namespace ci;
using namespace ci::app;

void OpenCLParticlesApp::setup()
{
	this->getWindow()->getSignalResize().connect(std::bind(&OpenCLParticlesApp::resize, this));
	
	fs::path path = this->getAppPath() / "Contents" / "Resources";
	this->addAssetDirectory(path);
	
	mShader = NULL;
	mKernelSystem = NULL;
	mBlurX = mBlurY = NULL;
	mEnableGaussianBlur = mMouseDown = false;
	
	mUseOpenGLContext = true;
	
//	Get a list of available platforms
//	Select device
//	Create Context
//	Create command queue
	
//	Create memory objects
//	Read kernel file
//	Create program object
//	Compile kernel
//	Create kernel object
//	Set kernel arguments
//	Execute kernel (Enqueue task)
//	Read memory object
//	Free objects
	
	// query platform
	cl_platform_id platformId = nullptr;
	cl_uint platformCount;
	cl_int ret = clGetPlatformIDs(1, &platformId, &platformCount);
	if (!ret) {
		console() << "OpenCLParticlesApp: Failed to query platform!" << std::endl;
		shutdown();
	}
	
	// make device query available to all future logic
	cl_uint deviceCount;
	cl_device_id deviceIds[4];
	
	// create context
	if (mUseOpenGLContext) {
		CGLContextObj cinderCglContext = this->getRenderer()->getCglContext();
		CGLShareGroupObj cinderCglShareGroup = CGLGetShareGroup(cinderCglContext);
		
		cl_context_properties ctxProperties[] = {
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties) cinderCglShareGroup,
			0
		};
		// create OpenCL context from shared CGL context
		mContext = clCreateContext(ctxProperties, 0, 0, clLogMessagesToStdoutAPPLE, 0, 0);
		
		if (!mContext) {
			console() << "OpenCLParticlesApp: Failed to create compute context!" << std::endl;
			shutdown();
		}
	}
	else {
		// query devices for platform
		ret = clGetDeviceIDs(platformId, CL_DEVICE_TYPE_DEFAULT, 1, deviceIds, &deviceCount);
		if (!ret) {
			console() << "OpenCLParticlesApp: Failed to get available devices!" << std::endl;
			shutdown();
		}
		
		// select device
		mContext = clCreateContext(0, deviceCount, deviceIds, nullptr, nullptr, &ret);
	}
	
	mCommandQueue = clCreateCommandQueue(mContext, (cl_device_id) deviceIds[0], 0, &ret);

	this->setWindowSize(1680,1080);
	this->setFrameRate(60);

	mKernelSystem = new KernelSystem();
	mEmitter = new Emitter( 1000, Vec2f::zero(), Vec2f::one() );
	mKernelSystem->setup( "resources/integrate-cpu.cl", 10000, mContext, mCommandQueue, deviceIds, deviceCount );
	
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

void OpenCLParticlesApp::addParticles( Vec2f pos, Vec2f vel )
{
	mEmitter->setPosition( pos );
	mEmitter->setDirection( vel );
	mKernelSystem->emit( *mEmitter );
}

void OpenCLParticlesApp::update()
{	
	mKernelSystem->update();
}

void OpenCLParticlesApp::draw()
{
	if (!mEnableGaussianBlur) {
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),true);
		mKernelSystem->draw();
	}
	else {
		// FIRST render pass...
		//============================================================
		mBlurX->bindFramebuffer();
		gl::clear();
		gl::color( ColorA::white() );
		gl::setMatricesWindow(getWindowWidth(),getWindowHeight(),false);
		mKernelSystem->draw();
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

void OpenCLParticlesApp::resize()
{
	int w = getWindowWidth();
	int h = getWindowHeight();
	
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

void OpenCLParticlesApp::keyDown( KeyEvent event )
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

void OpenCLParticlesApp::mouseMove( MouseEvent event )
{
	pMouse = event.getPos();
}

void OpenCLParticlesApp::mouseDown( MouseEvent event )
{
	mMouseDown = true;
}

void OpenCLParticlesApp::mouseUp( MouseEvent event )
{
	mMouseDown = false;
}

void OpenCLParticlesApp::mouseDrag( MouseEvent event )
{
	Vec2f mouseNorm = Vec2f( event.getPos() );
	Vec2f mouseVel = Vec2f( event.getPos() - pMouse ) * 0.25f;
	pMouse = event.getPos();
	this->addParticles(mouseNorm, mouseVel);
}

CINDER_APP_BASIC( OpenCLParticlesApp, RendererGl )
