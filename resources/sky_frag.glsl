#version 120

uniform samplerCube cubemap;

varying vec3 vNor; //normal in world space

void main() 
{
	gl_FragColor.rgb = textureCube(cubemap, vNor).rgb;
}