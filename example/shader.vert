#version 120

//uniform int offset;
//attribute float offset;

void main() {
	gl_FrontColor = gl_Color;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec3 normal = gl_Normal;
	normal = gl_NormalMatrix * normal;
	vec3 perp = normalize(cross(normal, vec3(0,0,-1)));
	
	vec4 vert = gl_Vertex;

	if(gl_TexCoord[0].t == 0) {
		vert = vert + 0.05 * vec4(perp, 0.0);
	} else if(gl_TexCoord[0].t == 1) {
		vert = vert - 0.05 * vec4(perp, 0.0);
	}

	gl_Position = gl_ModelViewProjectionMatrix * vert;
}
