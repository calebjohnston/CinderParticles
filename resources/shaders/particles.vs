uniform float time;

varying vec4 color;
varying vec3 velocity;
varying float start;

void main()
{
	gl_Position = MVPM * vert;
}