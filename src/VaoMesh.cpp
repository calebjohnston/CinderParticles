#include "VaoMesh.h"

#include "cinder/gl/gl.h"
using namespace cinder;

#if defined( CINDER_MAC )
	#define glBindVertexArray		glBindVertexArrayAPPLE
	#define glGenVertexArrays		glGenVertexArraysAPPLE
	#define glDeleteVertexArrays	glDeleteVertexArraysAPPLE
#endif 

VaoMesh::VaoMesh() 
:	mVaoId( 0 ),
	mNumIndices( 0 ),
	mNumVertices( 0 ),
	mPositionType( VaoMesh::POSITION_INVALID ),
	mVtxPositionLoc( INVALID_LOCATION ),
	mVtxColorLoc( INVALID_LOCATION ),
	mVtxTexCoordLoc( INVALID_LOCATION ),
	mVtxNormalLoc( INVALID_LOCATION ),
	mVtxIndexLoc( INVALID_LOCATION ),
	mVtxIndex2Loc( INVALID_LOCATION ),
	mVtxPositionEnabled( false ),
	mVtxIndexEnabled( false ),
	mVtxColorEnabled( false ),
	mVtxTexCoordEnabled( false ),
	mVtxNormalEnabled( false )
{
}

VaoMesh::~VaoMesh()
{
	destroyVao();
}

void VaoMesh::setup( POSITION_TYPE aPosType, gl::GlslProg& aShader )
{
	// Set position typoe
	mPositionType = aPosType;

	// Create VBOs
	mIndices = gl::Vbo( GL_ARRAY_BUFFER );
	mIndices2 = gl::Vbo( GL_ARRAY_BUFFER );
	mPosition = gl::Vbo( GL_ARRAY_BUFFER );
	mColor = gl::Vbo( GL_ARRAY_BUFFER );
	mTexCoord = gl::Vbo( GL_ARRAY_BUFFER );
	mNormal = gl::Vbo( GL_ARRAY_BUFFER );

	// Acquire the location of attributes from shader
	acquireAttributeLocations( aShader );

	// Create VAO
	createVao();

	// Enable the vertex attrib arrays
	enableVertexAttribArrays();
}

void VaoMesh::acquireAttributeLocations( ci::gl::GlslProg& aShader )
{
	mVtxIndexLoc	= glGetAttribLocation( aShader.getHandle(), "ciVtxIndex" );
	mVtxPositionLoc = glGetAttribLocation( aShader.getHandle(), "ciVtxPosition" );
	mVtxColorLoc	= glGetAttribLocation( aShader.getHandle(), "ciVtxColor" );
	mVtxTexCoordLoc = glGetAttribLocation( aShader.getHandle(), "ciVtxTexCoord" );
	mVtxNormalLoc	= glGetAttribLocation( aShader.getHandle(), "ciVtxNormal" );
}

void VaoMesh::enableVertexAttribArrays()
{
	// Disable everything first
	disableVertexAttribArrays();

	// Now enable what we need
	bindVao();

	// Position
	if( VaoMesh::INVALID_LOCATION != mVtxPositionLoc ) {
		if( VaoMesh::POSITION_VEC2 == mPositionType ) {
			mPosition.bind();
			//
			GLint size = sizeof(Vec2f) / sizeof(float);	// Should be 2
			GLsizei strideInBytes = sizeof(Vec2f);		// Should be 8
			glVertexAttribPointer( mVtxPositionLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
			glEnableVertexAttribArray( mVtxPositionLoc );
			//
			mPosition.unbind();
			
			mVtxPositionEnabled = true;
		}
		else if( VaoMesh::POSITION_VEC3 == mPositionType ) {
			mPosition.bind();
			//
			GLint size = sizeof(Vec3f) / sizeof(float);	// Should be 3
			GLsizei strideInBytes = sizeof(Vec3f);		// Should be 12
			glVertexAttribPointer( mVtxPositionLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
			glEnableVertexAttribArray( mVtxPositionLoc );
			//
			mPosition.unbind();

			mVtxPositionEnabled = true;
		}
	}

	// Color
	if( VaoMesh::INVALID_LOCATION != mVtxColorLoc ) {
		if( VaoMesh::POSITION_VEC2 == mPositionType ) {
			mColor.bind();
			//
			GLint size = sizeof(Color) / sizeof(float);	// Should be 3
			GLsizei strideInBytes = sizeof(Color);		// Should be 12
			glVertexAttribPointer( mVtxColorLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
			glEnableVertexAttribArray( mVtxColorLoc );
			//
			mColor.unbind();

			mVtxColorEnabled = true;
		}
	}

	// TexCoord
	if( VaoMesh::INVALID_LOCATION != mVtxTexCoordLoc ) {
		if( VaoMesh::POSITION_VEC2 == mPositionType ) {
			mTexCoord.bind();
			//
			GLint size = sizeof(Vec2f) / sizeof(float);	// Should be 2
			GLsizei strideInBytes = sizeof(Vec2f);		// Should be 8
			glVertexAttribPointer( mVtxTexCoordLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
			glEnableVertexAttribArray( mVtxTexCoordLoc );
			//
			mTexCoord.unbind();

			mVtxTexCoordEnabled = true;
		}
	}

	// Indices
	if( VaoMesh::INVALID_LOCATION != mVtxIndexLoc ) {
		mIndices.bind();
		//
		GLint size = 1;
		GLsizei strideInBytes = sizeof(float);
		glVertexAttribPointer( mVtxIndexLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
		glEnableVertexAttribArray( mVtxIndexLoc );
		//
		mIndices.unbind();

		mVtxIndexEnabled = true;
	}
	
	// Indices 2
	if( VaoMesh::INVALID_LOCATION != mVtxIndex2Loc ) {
		mIndices2.bind();
		//
		GLint size = 1;
		GLsizei strideInBytes = sizeof(Vec2f);
		glVertexAttribPointer( mVtxIndex2Loc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
		glEnableVertexAttribArray( mVtxIndex2Loc );
		//
		mIndices2.unbind();
		
		mVtxIndex2Enabled = true;
	}

	// Normal
	if( VaoMesh::INVALID_LOCATION != mVtxNormalLoc ) {
		if( VaoMesh::POSITION_VEC2 == mPositionType ) {
			mNormal.bind();
			//
			GLint size = sizeof(Vec3f) / sizeof(float);	// Should be 3
			GLsizei strideInBytes = sizeof(Vec3f);		// Should be 12
			glVertexAttribPointer( mVtxNormalLoc, size, GL_FLOAT, GL_FALSE, strideInBytes, 0 );
			glEnableVertexAttribArray( mVtxNormalLoc );
			//
			mNormal.unbind();

			mVtxNormalEnabled = true;
		}
	}

	unbindVao();
}

void VaoMesh::disableVertexAttribArrays()
{
	bindVao();
	
	// Indices
	if( mVtxIndexEnabled && ( VaoMesh::INVALID_LOCATION != mVtxIndexLoc ) ) {
		glDisableVertexAttribArray( mVtxIndexLoc );
		mVtxIndexEnabled = false;
	}

	// Position
	if( mVtxPositionEnabled && ( VaoMesh::INVALID_LOCATION != mVtxPositionLoc ) ) {
		glDisableVertexAttribArray( mVtxPositionLoc );
		mVtxPositionEnabled = false;
	}

	// Color
	if( mVtxColorEnabled && ( VaoMesh::INVALID_LOCATION != mVtxColorLoc ) ) {
		glDisableVertexAttribArray( mVtxColorLoc );
		mVtxColorEnabled = false;
	}

	// TexCoord
	if( mVtxTexCoordEnabled && ( VaoMesh::INVALID_LOCATION != mVtxTexCoordLoc ) ) {
		glDisableVertexAttribArray( mVtxTexCoordLoc );
		mVtxTexCoordEnabled = false;
	}

	// Normal
	if( mVtxNormalEnabled && ( VaoMesh::INVALID_LOCATION != mVtxNormalLoc ) ) {
		glDisableVertexAttribArray( mVtxNormalLoc );
		mVtxNormalEnabled = false;
	}

	unbindVao();
}


void VaoMesh::bindVao()
{
	glBindVertexArray( mVaoId );
}

void VaoMesh::unbindVao()
{
	glBindVertexArray( 0 );
}

void VaoMesh::createVao()
{
	if( 0 == mVaoId ) {
		glGenVertexArrays( 1, &mVaoId );
	}
}

void VaoMesh::destroyVao()
{
	if( 0 != mVaoId ) {
		glDeleteVertexArrays( 1, &mVaoId );
	}
}

void VaoMesh::bufferIndices( const std::vector<float>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(float);
	mIndices.bind();
	mIndices.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mIndices.unbind();

	// Set number of indices
	mNumIndices = (int)aData.size();
}

void VaoMesh::bufferIndices2( const std::vector<Vec2f>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Vec2f);
	mIndices2.bind();
	mIndices2.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mIndices2.unbind();
}

void VaoMesh::bufferPosition2( const std::vector<Vec2f>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Vec2f);
	mPosition.bind();
	mPosition.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mPosition.unbind();

	// Set number of vertices
	mNumVertices = (int)aData.size();
}

void VaoMesh::bufferPosition3( const std::vector<Vec3f>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Vec3f);
	mPosition.bind();
	mPosition.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mPosition.unbind();

	// Set number of vertices
	mNumVertices = (int)aData.size();
}

void VaoMesh::bufferColor( const std::vector<Color>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Color);
	mColor.bind();
	mColor.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mColor.unbind();
}

void VaoMesh::bufferTexCoord( const std::vector<Vec2f>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Vec2f);
	mTexCoord.bind();
	mTexCoord.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mTexCoord.unbind();
}

void VaoMesh::bufferNormal( const std::vector<Vec3f>& aData )
{
	size_t sizeInBytes = aData.size()*sizeof(Vec3f);
	mNormal.bind();
	mNormal.bufferData( sizeInBytes, &(aData[0]), GL_STATIC_DRAW );
	mNormal.unbind();
}
