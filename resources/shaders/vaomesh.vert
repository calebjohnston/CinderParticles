// Attributes
attribute vec3 ciVtxPosition;
attribute vec2 ciVtxTexCoord;
attribute vec2 ciVtxIndex;

// Uniforms
uniform mat4 ciModelViewProjectionMatrix;
uniform sampler2D ciTranslation;
uniform sampler2D ciRotation;
uniform sampler2D ciScale;

// Varying
//varying vec2 TexCoord;

void main()
{
	vec3 position = ciVtxPosition;
	vec3 t = texture2D( ciTranslation, ciVtxIndex).rgb;
	vec3 r = texture2D( ciRotation, ciVtxIndex).rgb;
	vec3 s = texture2D( ciScale, ciVtxIndex).rgb;
	
	mat4 modelview;
	modelview[0][0] = 1.0;
	modelview[1][1] = 1.0;
	modelview[2][2] = 1.0;
	modelview[3][3] = 1.0;
	modelview[3][1] = 0.0;	// = 1.0 ????
//	modelview[3] = vec4(t, 1.0);
	
	mat4 composedMatrix = ciModelViewProjectionMatrix * modelview;
	
	gl_Position = composedMatrix * vec4( position, 1.0 );
	//TexCoord = ciVtxTexCoord;
}
