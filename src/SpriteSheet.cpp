#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"

#include "SpriteSheet.h"

using namespace ci;
using namespace std;

SpriteSheet::SpriteSheet()
:	mCurrentFrameIndex(0),
	mElapsedFrameTime(0.0),
	mShouldLoop(false), mIsPaused(false), mIsReversed(false)
{
}

SpriteSheet::SpriteSheet(const XmlTree& coords, const gl::Texture& texture, int frameRate) :
	mCurrentFrameIndex(0),
	mElapsedFrameTime(0.0),
	mShouldLoop(false), mIsPaused(true), mIsReversed(false)
{
	XmlTree textureAtlas = coords.getChild("TextureAtlas");
	int i = 0;
	mFrameRate = 1.0f / (float)frameRate;

	mTexture = texture;

	for(XmlTree::Iter child = textureAtlas.begin(); child != textureAtlas.end(); ++child) {
		float x = child->getAttributeValue<float>("x");
		float y = child->getAttributeValue<float>("y");
		float width = child->getAttributeValue<float>("width");
		float height = child->getAttributeValue<float>("height");

		if (i++ == 0) {
			mDrawSize = Rectf(0, 0, width, height);
			mSpriteFrameSize = Vec2f(width / mTexture.getWidth(), height / mTexture.getHeight());
		}

		mFrames.push_back(Vec2f(x / mTexture.getWidth(), y / mTexture.getHeight()));
	}

	mElapsedTime = ci::app::App::get()->getElapsedSeconds();
	mElapsedFrameTime = 0.0f;
}

SpriteSheet::~SpriteSheet()
{
}

int SpriteSheet::numFrames() const
{
	return mFrames.size();
}

void SpriteSheet::addStopAtKeyFrame(const int frame)
{
	int clampedFrame = math<int>::clamp(frame, 0, this->numFrames());
	mStopAtKeyFrames.push_back(clampedFrame);
}

void SpriteSheet::addResetAtKeyFrame(const int frame)
{
	int clampedFrame = math<int>::clamp(frame, 0, this->numFrames());
	mResetAtKeyFrames.push_back(clampedFrame);
}

void SpriteSheet::update()
{
	float currentTime = ci::app::App::get()->getElapsedSeconds();
	float deltaTime = currentTime - mElapsedTime;
	mElapsedTime = currentTime;
	
	if (!mIsPaused)
	{
		mElapsedFrameTime += deltaTime;
		float timeDiff = mElapsedFrameTime - (float)mFrameRate;
		if (timeDiff >= 0) {
			mElapsedFrameTime = timeDiff;
			if (mIsReversed) {
				if (mCurrentFrameIndex-- < 0) {
					if (!mShouldLoop) {
						mIsPaused = true;
						mCurrentFrameIndex = 0;
					}
					else mCurrentFrameIndex = mFrames.size()-1;
				}
			}
			else {
				if (mCurrentFrameIndex++ >= mFrames.size()-1) {
					if (!mShouldLoop) {
						mIsPaused = true;
						mCurrentFrameIndex = mFrames.size()-1;
					}
					else mCurrentFrameIndex = 0;
				}
			}
			
			std::vector<int>::iterator k;
			
			// Check stop at key frames
			for(k = mStopAtKeyFrames.begin(); k != mStopAtKeyFrames.end(); k++)
				if (mCurrentFrameIndex == *k)
					mIsPaused = true;
			
			// Check reset at key frames
			for(k = mResetAtKeyFrames.begin(); k != mResetAtKeyFrames.end(); k++) {
				if (mCurrentFrameIndex == *k) {
					if (mIsReversed) mCurrentFrameIndex = mFrames.size()-1;
					else mCurrentFrameIndex = 0;
				}
			}
		}
	}
}

// accept a float for a set time...

void SpriteSheet::gotoAndPlay(const int frame)
{
	this->gotoAndStop(frame);
	mElapsedFrameTime = 0.0f;
	this->play();
}

void SpriteSheet::randomFrame()
{
	int rnd = Rand::randFloat() * this->numFrames();
	if (mIsPaused) this->gotoAndStop(rnd);
	else this->gotoAndPlay(rnd);
}

void SpriteSheet::randomFrame(const int min, const int max)
{
	int rnd = min + Rand::randFloat() * (max - min);
	if (mIsPaused) this->gotoAndStop(rnd);
	else this->gotoAndPlay(rnd);
}

void SpriteSheet::gotoAndStop(const int frame)
{
	mCurrentFrameIndex = math<int>::clamp(frame, 0, this->numFrames());
	this->pause();
}

void SpriteSheet::play()
{
	mIsPaused = false;
}

void SpriteSheet::pause()
{
	mIsPaused = true;
}

void SpriteSheet::draw()
{
	Vec2f* f = &mFrames[mCurrentFrameIndex];
	
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glTranslatef(f->x, f->y, 0.0);
	glScalef(mSpriteFrameSize.x, mSpriteFrameSize.y, 0.0);
	gl::draw(mTexture, mDrawSize);
	glPopMatrix();
}