#version 120

//uniform int offset;
//attribute float offset;
//uniform vec3 lookvec;

uniform float lineWidth;

void main() {
	gl_FrontColor = gl_Color;
	
	gl_TexCoord[0] = gl_MultiTexCoord0;

	vec3 normal = gl_Normal;
	vec3 lookvec = normalize(vec3(gl_ModelViewMatrixInverse * vec4(0.0, 0.0, -1.0, 0.0)));
	vec3 perp = normalize(cross(normal, lookvec));
	
	vec4 vert = gl_Vertex;


	if(gl_TexCoord[0].t == 0) {
		vert = vert + lineWidth * vec4(perp, 0.0);
	} else if(gl_TexCoord[0].t == 1) {
		vert = vert - lineWidth * vec4(perp, 0.0);
	}

	gl_Position = gl_ModelViewProjectionMatrix * vert;
}
