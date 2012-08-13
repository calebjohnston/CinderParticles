#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"

#include "SpriteSheetTestApp.h"

using namespace ci;
using namespace ci::app;

void SpriteSheetTestApp::setup()
{
	fs::path path = this->getAppPath() / "Contents" / "Resources";
	this->addAssetDirectory(path);
	
	path = this->getAppPath() / ".." / ".." / "resources";
	this->addAssetDirectory(path);
	
	XmlTree config( loadAsset("dk.xml") );
	
	mSheet = gl::Texture(loadImage( App::loadAsset("images/dk.png") ));
	
	gl::enableAlphaBlending();
	
	mSprites = SpriteSheet(config, mSheet, 14);
	mSprites.setLoop(true);
	mSprites.play();
}

void SpriteSheetTestApp::update()
{
	mSprites.update();
}

void SpriteSheetTestApp::draw()
{
	gl::clear();
	gl::color(1,1,1,1);
	mSprites.draw();
}

void SpriteSheetTestApp::resize( ResizeEvent event )
{
}

void SpriteSheetTestApp::keyDown( KeyEvent event )
{
}

void SpriteSheetTestApp::mouseMove( MouseEvent event )
{
}

void SpriteSheetTestApp::mouseDown( MouseEvent event )
{
}

void SpriteSheetTestApp::mouseUp( MouseEvent event )
{
}

void SpriteSheetTestApp::mouseDrag( MouseEvent event )
{
}

CINDER_APP_BASIC( SpriteSheetTestApp, RendererGl )
