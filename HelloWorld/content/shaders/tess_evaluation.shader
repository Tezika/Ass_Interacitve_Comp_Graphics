#version 430 core

layout (triangles) in;

in vec4 tc_position[];
in vec2 tc_texCoord[];

out vec3 fragPos;
out vec3 normal;
out vec3 lightPos;

void main(void)
{
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position) + (gl_TessCoord.y * gl_in[1].gl_Position) +(gl_TessCoord.z * gl_in[2].gl_Position);
    vec2 te_texCoord =  (gl_TessCoord.x * tc_texCoord[0] + gl_TessCoord.y * tc_texCoord[1] + gl_TessCoord.z * tc_texCoord[2]);
}