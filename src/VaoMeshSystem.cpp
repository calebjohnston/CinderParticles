#include "VaoMeshSystem.h"

#include "cinder/ObjLoader.h"
#include "cinder/app/App.h"
#include "cinder/ImageIO.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;

VaoMeshSystem::VaoMeshSystem(const std::string& filepath, const gl::GlslProg& shader)
 :	ParticleSystem(), 
	mParticles(NULL),
	mColorArray(NULL),
	mPositionArray(NULL),
	mNumCache(NULL),
	mMesh(NULL),
	mVao(NULL),
	mShader(shader)
{
	mMesh = new TriMesh();
	ObjLoader loader( (DataSourceRef) app::loadAsset(filepath) );
	loader.load( mMesh );
}

VaoMeshSystem::~VaoMeshSystem() 
{
	delete[] mParticles;
	delete mNumCache;
	delete mMesh;
	delete mVao;
}

void VaoMeshSystem::setup(const unsigned int particles, const int threads)
{
	ParticleSystem::setup(particles, threads);
	unsigned int side_size = (unsigned int) math<float>::sqrt(mMaxParticles);
	if(particles < side_size * side_size) side_size += 1;
	mMaxParticles = side_size * side_size;
	
	// allocate memory
	try {
		mParticles = (VaoMeshParticle*) calloc(sizeof(VaoMeshParticle), mMaxParticles);
	} catch(...) {
		app::console() << "Unable to allocate data" << std::endl;
		return;
	}
	
	// create surface to write values to
	mMeshData = Surface32f( side_size, side_size, true);
	Surface32f::Iter pixelIter = mMeshData.getIter();
	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			mMeshData.setPixel( pixelIter.getPos(), ColorA::black() );
		}
	}
	// create texture to perform VTF in
	mMeshTexture = gl::Texture( mMeshData );
	
	// create VAO object
	mVao = new VaoMesh();
	mVao->setup(VaoMesh::POSITION_VEC3, mShader);
	
	// Prepare VAO with mesh indices 
	// Assuming one pixel per mesh for now. Needs to be more flexible
	std::vector<Vec2f> indices;
	indices.resize(mMaxParticles);
	for( int x = 0; x < side_size; ++x ) {
		for( int y = 0; y < side_size; ++y ) {
			Vec2f st( x/(float)side_size, y/(float)side_size );
			indices.push_back(st);
		}
	}
	mVao->bufferIndices2(indices);
	
	// buffer all other VAO mesh data
	std::vector<Vec3f> points;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texCoords;
	points.resize(mMesh->getNumVertices() * mMaxParticles);
	texCoords.resize(mMesh->getNumVertices() * mMaxParticles);
	normals.resize(mMesh->getNumVertices() * mMaxParticles);
	for(unsigned int i=0; i<mMaxParticles; i++){
		points.insert(points.begin(), mMesh->getVertices().begin(), mMesh->getVertices().end());
		normals.insert(normals.begin(), mMesh->getNormals().begin(), mMesh->getNormals().end());
		texCoords.insert(texCoords.begin(), mMesh->getTexCoords().begin(), mMesh->getTexCoords().end());
	}
	mVao->bufferPosition3(points);
	mVao->bufferPosition3(normals);
	mVao->bufferTexCoord(texCoords);
	
	for(unsigned int i=0; i<mMaxParticles; i++){
		mParticles[mCurrentIndex++].init(Vec3f::zero(), Vec3f::zero());
	}
	
	mWindowSize = app::App::get()->getWindowSize();
	mInvWindowSize = Vec2f( 1.0f / mWindowSize.x, 1.0f / mWindowSize.y );
	
	// setup NumberCache
	mNumCache = new NumberCache(2048);	// maximum size of 2 kilobytes
	mNumCache->computeRandomVectors();
	
	mInitialized = true;
}

void VaoMeshSystem::updateKernel(const unsigned int start_index, const unsigned int end_index)
{
	if(!mInitialized || !mVao) return;
	
	for(size_t index = start_index; index < end_index; index++){
		VaoMeshParticle* mesh = mParticles + index;
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
		mesh->color.r = mesh->color.a * 0.25f;;
		mesh->color.g = mesh->color.a * 0.75f;
		mesh->color.b = 0.5 - mesh->color.a * 0.35f;// 1.0 - mesh->color.a;
		mesh->color.a -= 0.01;
		
		// write shit to the image
	}
}

void VaoMeshSystem::update()
{	
	if(!mInitialized || !mVao) return;
	
	mNumCache->computeRandomVectors();	// update number cache
	
	// executes compute kernel (or relies upon threads to do so)
	ParticleSystem::update();
}

void VaoMeshSystem::emit(const Emitter& emitter)
{
	if(!mInitialized || !mVao) return;
	
	Vec2f pos = emitter.getPosition();
	Vec2f vel = emitter.getDirection();
	for(unsigned int i=0; i<emitter.getRate(); i++){
		Vec2f p = mNumCache->nextPosition();
		Vec2f v = mNumCache->nextVelocity();
		mParticles[mCurrentIndex++].init(Vec3f(pos.x + p.x, pos.y + p.y, 0.0),
										 Vec3f(vel.x + v.x, vel.y + v.y, 0.0), 
										 Vec3f(10.0f,10.0f,10.0f), 
										 Vec3f(-0.05f, -0.07f, -0.03f), 
										 Vec3f(0.0,1.0,0), 0.1f, 1.0f,
										 ColorA::white());
		
		if(mCurrentIndex >= mMaxParticles){
			mCurrentIndex = 0;
		}
	}
}

void VaoMeshSystem::draw()
{	
	if(!mInitialized || !mVao) return;
	
	ParticleSystem::preDraw();
	
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	// bind shader and add uniforms...
	mShader.bind();
	mShader.uniform("tex0???", 0);
	
	// bind texture(s)
	mMeshTexture.bind(0);
	
	mVao->bindVao();
	glDrawArrays( GL_TRIANGLES, 0, mVao->getNumVertices() );
	mVao->unbindVao();
	
	mMeshTexture.unbind(0);
	mShader.unbind();
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	
	ParticleSystem::postDraw();
}
