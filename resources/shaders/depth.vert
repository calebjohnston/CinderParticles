#version 110

varying vec4 V;

void main()
{
	gl_Position = ftransform();
	V = gl_Position;
}