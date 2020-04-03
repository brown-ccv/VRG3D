#version 120
#extension GL_EXT_geometry_shader4 : enable

varying in vec3 normal[];
varying out vec3 oNormal;

layout(lines) in;
layout(triangle_strip, max_vertices = 6) out;

void main(void) {
/*	gl_FrontColor = gl_Color(1.0, 1.0, 1.0);
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_Position = gl_PositionIn[0];
	EmitVertex();

	gl_FrontColor = gl_Color(1.0, 1.0, 1.0);
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_Position = gl_PositionIn[1];
	EmitVertex();

	gl_FrontColor = gl_Color(1.0, 1.0, 1.0);
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_Position = gl_PositionIn[0] - gl_PositionIn[1]*0.5;
	EmitVertex();
	EndPrimitive();
	*/
	vec3 perp = normalize(cross(vec3(gl_PositionIn[1] - gl_PositionIn[0]), vec3(0,0,1)));
	float width = 0.05;

	gl_FrontColor = gl_FrontColorIn[1];
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	vec3 perpNorm = normalize(cross(normal[1], vec3(0,0,1)));
	vec3 pos = (width/(dot(perp, perpNorm))) * perpNorm + vec3(0.0, 0.0, -0.1);
	//gl_Position = gl_PositionIn[1] + vec4(v * 0.05, 0.0);
	gl_Position = gl_PositionIn[1] + vec4(pos, 0.0);
	EmitVertex();

	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = gl_TexCoordIn[0][0];
	perpNorm = normalize(cross(normal[0], vec3(0,0,1)));
	pos = (width/(dot(perp, perpNorm))) * perpNorm + vec3(0.0, 0.0, -0.1);
	gl_Position = gl_PositionIn[0] + vec4(pos, 0.0);
	EmitVertex();
	
	gl_FrontColor = gl_FrontColorIn[1];
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_Position = gl_PositionIn[1];
	EmitVertex();

	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = gl_TexCoordIn[0][0];
	gl_Position = gl_PositionIn[0];
	EmitVertex();

//	perp = vec3(0.0, 0.0, 0.2)-perp;
	gl_FrontColor = gl_FrontColorIn[1];
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	perpNorm = normalize(cross(normal[1], vec3(0,0,1)));
	pos = (width/(dot(perp, perpNorm))) * perpNorm + vec3(0.0, 0.0, -0.1);
	gl_Position = gl_PositionIn[1] - vec4(pos, 0.0);
	EmitVertex();

	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = gl_TexCoordIn[0][0];
	perpNorm = normalize(cross(normal[0], vec3(0,0,1)));
	pos = (width/(dot(perp, perpNorm))) * perpNorm + vec3(0.0, 0.0, -0.1);
	gl_Position = gl_PositionIn[0] - vec4(pos, 0.0);
	EmitVertex();
	EndPrimitive();
}

