#version 120

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 Mit;
uniform float time;
uniform int switchPos;

attribute vec4 aPos;
attribute vec3 aNor;

varying vec3 vPos;
varying vec3 vNor;

void main()
{
	vPos = (M * aPos).xyz;
	vec3 wPos = vPos;
	if (switchPos == 1) {
		vPos.x += 2 * cos(time);
		vPos.z += 2 * sin(time);
	}
	if (switchPos == 2) {
		vPos.z += 3 * cos(time);
		vPos.y += 3 * sin(time);
	}
	if (switchPos == 3) {
		vPos.x += 4 * cos(time);
		vPos.y += 4 * sin(time);
		vPos.z += 4 * cos(time);
	}

	vNor = normalize((M * vec4(aNor, 0.0)).xyz);
	gl_Position = P * (V * vec4(vPos, 1.0));
}