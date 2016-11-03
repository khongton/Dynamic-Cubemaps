#version 120

uniform mat4 P;
uniform mat4 MV;

attribute vec4 aPos;
attribute vec3 aNor;

varying vec3 vNor; //normal in world space

void main() 
{
	vNor = aNor;
	gl_Position = P * MV * aPos;
}