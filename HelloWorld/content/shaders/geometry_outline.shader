#version 330 core
layout(triangles) in;
layout(line_strip, max_vertices = 4) out;

void main() {
	for(int i = 0; i < 4; i++) {
		vec4 curPos = gl_in[i % 3].gl_Position;
		gl_Position = vec4(curPos.x, curPos.y, curPos.z - 0.1, curPos.w);
		EmitVertex();
	}
	EndPrimitive();
}