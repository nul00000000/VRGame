#version 400 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 colorPass[];
in vec3 pNormal[];
in vec3 fragPos[];

flat out vec3 colors[3];
flat out vec3 coords[3];
flat out vec3 normal;
out vec3 pos;


void main(void) {
	for(int i = 0; i < 3; ++i) {
		colors[i] = colorPass[i];
		coords[i] = fragPos[i];
	}
	for(int i = 0; i < 3; ++i) {
		normal = pNormal[i];
		pos = fragPos[i];
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
}