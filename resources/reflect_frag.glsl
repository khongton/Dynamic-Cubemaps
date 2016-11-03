#version 120

uniform vec3 cPos;	//camera position in world coords
uniform samplerCube cubemap;

varying vec3 vPos; //fragment pos in world
varying vec3 vNor; //fragment norm in world

void main() 
{
	//eye vector
	vec3 e = normalize(cPos - vPos);
	//normal vector
	vec3 n = normalize(vNor);
	//reflected vector
	vec3 reflected = reflect(-e, n);
	//sample color from cubemap
	gl_FragColor.rgb = textureCube(cubemap, reflected).rgb;
}