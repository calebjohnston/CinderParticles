#pragma once

#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Xml.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Rect.h"

class SpriteSheet {
public:
	SpriteSheet();
	SpriteSheet(const ci::XmlTree& coords, const ci::gl::Texture& texture, int frameRate);
	~SpriteSheet();
	void update();
	void draw();
	void pause();
	void play();
	void playBackwards();
	void gotoAndPlay(const int frame);
	void gotoAndStop(const int frame);

	int currentFrame() const { return mCurrentFrameIndex; }
	int numFrames() const;

	ci::Vec2f size() const { return ci::Vec2f(mDrawSize.x2, mDrawSize.y2); }
	ci::Vec2f frameSize() const { return ci::Vec2f(mDrawSize.x2, mDrawSize.y2); }

	bool isPaused() const { return mIsPaused; }

	/* Goto a random frame between 0 and numFrames-1 */
	void randomFrame();

	/* Goto a random frame between the min and max params specified */
	void randomFrame(const int min, const int max);

	/* Play in reverse (i.e. cycle through frames backrwards) */
	void reverse() { mIsReversed = !mIsReversed; }

	/* When the current frame reaches one of these key frames, the spritesheet will pause */
	void addStopAtKeyFrame(const int frame);

	/* When the current frame reaches one fo these key frames, the sprite sheet will loop to the begining (or end if reversed) */
	void addResetAtKeyFrame(const int frame);

	/* If true, the sprite sheet will play at frame zero once that last frame is reached, or vice versa if reversed */
	void setLoop(const bool loop) { mShouldLoop = loop; }

private:
	bool mIsPaused;
	bool mIsReversed;
	bool mShouldLoop;

	std::vector<int> mStopAtKeyFrames;
	std::vector<int> mResetAtKeyFrames;

	ci::gl::Texture mTexture;
	std::vector<ci::Vec2f> mFrames;
	ci::Vec2f mSpriteFrameSize;
	ci::Rectf mDrawSize;
	ci::Vec4f mCurrentFrame;
	int mCurrentFrameIndex;
	float mFrameRate;
	float mElapsedFrameTime;
	float mElapsedTime;
	
};