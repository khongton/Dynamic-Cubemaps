#version 120

uniform vec3 cPos;
uniform vec3 lightPos;
uniform samplerCube cubemap;

varying vec3 vPos;
varying vec3 vNor;

void main()
{
	//blinn-phong lighting
	vec3 ka = vec3(0.2, 0.2, 0.2);
	vec3 kd = vec3(0.8, 0.7, 0.7);
	vec3 ks = vec3(1.0, 0.9, 0.8);
	float s = 200.0;

	vec3 n = normalize(vNor);
	vec3 lightDir = normalize(lightPos - vPos);
	vec3 viewDir = normalize(cPos - vPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = max(0, dot(halfDir, n));
	float totalContrib = pow(spec, s);

	vec3 color = ka + kd * max(0, dot(lightDir, n)) + ks * totalContrib;
	//vec3 e = normalize(cPos - vPos);
	//vec3 n = normalize(vNor);
	//vec3 reflected = reflect(-e, n);
	//gl_FragColor.rgb = textureCube(cubemap, reflected).rgb;
	gl_FragColor = vec4(color, 1.0);
}