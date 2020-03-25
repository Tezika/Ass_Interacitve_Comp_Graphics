#version 430 core

layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

in vec4 te_position[];
in vec3 te_normal[];
in vec2 te_texCoord[];

out vec3 fragPos;
out vec3 normal;
out vec3 lightPos;
out vec2 uv;

void main() {
	for(int i = 0; i < 4; i++) {
		gl_Position = gl_in[i % 3].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}