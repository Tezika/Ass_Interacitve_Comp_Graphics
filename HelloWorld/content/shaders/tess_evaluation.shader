#version 430 core

layout (triangles) in;

in vec4 tc_position[];
in vec3 tc_normal[];
in vec2 tc_texCoord[];

out vec4 te_position;
out vec3 te_normal;
out vec2 te_texCoord;

void main(void)
{
    te_position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) +(gl_TessCoord.z * gl_in[2].gl_Position);
    gl_Position = te_position; 
    te_texCoord =  (gl_TessCoord.x * tc_texCoord[0] + gl_TessCoord.y * tc_texCoord[1] + gl_TessCoord.z * tc_texCoord[2]);
	te_normal = (gl_TessCoord.x * tc_normal[0] + gl_TessCoord.y * tc_normal[1] + gl_TessCoord.z * tc_normal[2]);
}