#version 110

varying vec4 V;

void main()
{
	float depth = V.z / V.w;
	gl_FragColor = vec4( depth, depth, depth, 1.0 );
}

// Note: I use a floating point render target in this case. For a standard texture, you'll 
// have to pack the depth value: float depth = (V.z / V.w) * 0.5 + 0.5;