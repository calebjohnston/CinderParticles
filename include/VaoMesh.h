#pragma once

#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Vbo.h"
#include "cinder/TriMesh.h"

/**
 * \class VaoMesh
 *
 */
class VaoMesh {
public:

	enum POSITION_TYPE {
		POSITION_INVALID,
		POSITION_VEC2,
		POSITION_VEC3
	};

	enum {
		INVALID_LOCATION = -1
	};

	VaoMesh();
	virtual ~VaoMesh();

	int				getNumVertices() const { return mNumVertices; }
	
	int				getNumIndices() const { return mNumIndices; }

	void			setup( POSITION_TYPE aPosType, ci::gl::GlslProg& aShader );

	void			bindVao();
	void			unbindVao();

	void			acquireAttributeLocations( ci::gl::GlslProg& aShader );
	void			enableVertexAttribArrays();
	void			disableVertexAttribArrays();

	void			bufferIndices( const std::vector<float>& aData );
	void			bufferIndices2( const std::vector<ci::Vec2f>& aData );
	void			bufferPosition2( const std::vector<ci::Vec2f>& aData );
	void			bufferPosition3( const std::vector<ci::Vec3f>& aData );
	void			bufferColor( const std::vector<ci::Color>& aData );
	void			bufferTexCoord( const std::vector<ci::Vec2f>& aData );
	void			bufferNormal( const std::vector<ci::Vec3f>& aData );

private:
	GLuint			mVaoId;

	int				mNumVertices;
	int				mNumIndices;
	POSITION_TYPE	mPositionType;

	ci::gl::Vbo		mIndices;
	ci::gl::Vbo		mIndices2;
	ci::gl::Vbo		mPosition;
	ci::gl::Vbo		mColor;
	ci::gl::Vbo		mTexCoord;
	ci::gl::Vbo		mNormal;
	
	int				mVtxIndexLoc;
	int				mVtxIndex2Loc;
	int				mVtxPositionLoc;
	int				mVtxColorLoc;
	int				mVtxTexCoordLoc;
	int				mVtxNormalLoc;

	bool			mVtxPositionEnabled;
	bool			mVtxIndexEnabled;
	bool			mVtxIndex2Enabled;
	bool			mVtxColorEnabled;
	bool			mVtxTexCoordEnabled;
	bool			mVtxNormalEnabled;

	void			createVao();
	void			destroyVao();
};
