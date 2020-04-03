#version 120
#extension GL_EXT_geometry_shader4 : enable

// This is a passthrough geometry shader that takes in Triangle primitives
// and outputs Triangle Strip primitives

void main() {
	gl_FrontColor = gl_FrontColorIn[0];
	gl_TexCoord[0] = gl_TexCoordIn[0][0];
	gl_Position = gl_PositionIn[0];
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[1];
	gl_TexCoord[0] = gl_TexCoordIn[1][0];
	gl_Position = gl_PositionIn[1];
	EmitVertex();
	gl_FrontColor = gl_FrontColorIn[2];
	gl_TexCoord[0] = gl_TexCoordIn[2][0];
	gl_Position = gl_PositionIn[2];
	EmitVertex();
	EndPrimitive();
}
