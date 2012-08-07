#include "MeshSystem.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

MeshSystem::MeshSystem(const std::string& filepath)
 :	ParticleSystem(), 
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL),
	mMesh(NULL),
	mVBO(NULL)
{		
	mMesh = new TriMesh();
	ObjLoader loader( (DataSourceRef) app::loadAsset(filepath) );
	loader.load( mMesh );
	mVBO = new gl::VboMesh( *mMesh );
}

MeshSystem::~MeshSystem() 
{
	delete[] mParticles;
	delete mNumCache;
	delete mMesh;
	delete mVBO;
}

void MeshSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	
	// allocate memory
	try {
		mParticles = (MeshParticle*) calloc(sizeof(MeshParticle), mMaxParticles);
	} catch(...) {
		app::console() << "Unable to allocate data" << std::endl;
		return;
	}
	
	for(unsigned int i=0; i<mMaxParticles; i++){
		mParticles[mCurrentIndex++].init(mVBO, Vec3f::zero(), Vec3f::zero());
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// maximum size of 2 kilobytes
	mNumCache->computeRandomVectors();
	
	mInitialized = true;
}

void MeshSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized && mVBO) return;
	
	for(size_t index = start_index; index < end_index; index++){
		MeshParticle* mesh = mParticles + index;
		// accumulate system forces
		mesh->velocity.y += 0.15;
		
		// integrate rotation
		mesh->rotationAngle += mesh->rotationalVelocity;
		mesh->rotationAxis.x += 0.13f;
		mesh->rotation.set(mesh->rotationAxis, mesh->rotationAngle);
		
		// integrate scale
		mesh->scale += mesh->tensor;
		
		// integrate position
		mesh->position += mesh->velocity;
		
		// fade out and update color
		mesh->color.r = mesh->color.a;
		mesh->color.g = mesh->color.a * 0.45f;
		mesh->color.b = 0.5 - mesh->color.a * 0.5f;// 1.0 - mesh->color.a;
		mesh->color.a -= 0.01;
	}
}

void MeshSystem::update()
{	
	if(!mInitialized && mVBO) return;
	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void MeshSystem::emit(const Emitter& emitter)
{
	if(!mInitialized && mVBO) return;
	
	this->addParticles(emitter.getRate(), emitter.getPosition(), emitter.getDirection());
}

void MeshSystem::addParticles(const unsigned int amount, const Vec2f& pos, const Vec2f& vel)
{
	if(!mInitialized && mVBO) return;
	
	for(unsigned int i=0; i<amount; i++){
		Vec2f p = mNumCache->nextPosition();
		Vec2f v = mNumCache->nextVelocity();
		mParticles[mCurrentIndex++].init(mVBO, 
										 Vec3f(pos.x + p.x, pos.y + p.y, 0.0),
										 Vec3f(vel.x + v.x, vel.y + v.y, 0.0), 
										 Vec3f(6.0f,6.0f,6.0f), 
										 Vec3f(-0.05f, -0.07f, -0.03f), 
										 Vec3f(0.0,1.0,0), 0.1f, 1.0f,
										 ColorA::white());
		
		if(mCurrentIndex >= mMaxParticles){
			mCurrentIndex = 0;
		}
	}
}

void MeshSystem::draw()
{	
	if(!mInitialized && mVBO) return;
	
	ParticleSystem::preDraw();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	for(size_t i=0; i<mMaxParticles; i++){
		gl::pushModelView();
		gl::color(mParticles[i].color);
		gl::translate(mParticles[i].position);
		gl::rotate(mParticles[i].rotation);
		gl::scale(mParticles[i].scale);
		gl::draw(*(mParticles[i].mesh));
		gl::popModelView();
	}
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	ParticleSystem::postDraw();
}
