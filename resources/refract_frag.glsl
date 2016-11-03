#version 120

uniform vec3 cPos; // camera position in world coords
uniform samplerCube cubemap;

varying vec3 vPos; // fragment position in world coords
varying vec3 vNor; // fragment normal in world coords

vec3 eta = vec3(1.05, 1.06, 1.07);

// Reflect and refract
vec3 reflect_refract(vec3 e, vec3 n)
{
	// NOTE: There are lots of repeated computations!
	// It's not efficient to call refract() and reflect() multiple times.
	vec3 vr = refract(-e, n, eta.r);
	vec3 vg = refract(-e, n, eta.g);
	vec3 vb = refract(-e, n, eta.b);
	float cr = textureCube(cubemap, normalize(vr)).r;
	float cg = textureCube(cubemap, normalize(vg)).g;
	float cb = textureCube(cubemap, normalize(vb)).b;
	vec3 refr = vec3(cr, cg, cb);
	vec3 refl = textureCube(cubemap, reflect(-e, n)).rgb;
	float alpha = 1.0 - dot(e, n);
	return mix(refr, refl, alpha);
}

void main()
{
	vec3 e = normalize(cPos - vPos);
	vec3 n = normalize(vNor);
	gl_FragColor.rgb = reflect_refract(e, n);
}
