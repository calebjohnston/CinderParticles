#pragma once

#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"

#include "SpriteSheet.h"

class SpriteSheetTestApp : public ci::app::AppBasic {
public:
	void	setup();
	void	update();
	void	draw();
	void	keyDown( ci::app::KeyEvent event );
	void	mouseMove( ci::app::MouseEvent event );
	void	mouseDrag( ci::app::MouseEvent event );
	void	mouseDown( ci::app::MouseEvent event );
	void	mouseUp( ci::app::MouseEvent event );
	void	resize();

private:
	SpriteSheet mSprites;
	ci::gl::Texture mSheet;
};
