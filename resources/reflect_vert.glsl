#version 120

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 Mit;

//model space
attribute vec4 aPos;
attribute vec3 aNor;

//In world coords
varying vec3 vPos;
varying vec3 vNor;

void main()
{
	//put vertex pos into world coordinates
	vPos = (M * aPos).xyz;
	//vertex normal in world coords
	vNor = normalize((Mit * vec4(aNor, 0.0)).xyz);
	//vertex position in camera space
	gl_Position = P * (V * vec4(vPos, 1.0));
}