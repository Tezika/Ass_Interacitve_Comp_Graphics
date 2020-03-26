#version 430 core

layout (vertices = 3) out;

uniform float level_tess;

in vec4 v_position[];
in vec2 v_texCoord[];

out vec4 tc_position[];
out vec2 tc_texCoord[];

void main(void)
{

    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = level_tess;
        gl_TessLevelOuter[0] = level_tess;
        gl_TessLevelOuter[1] = level_tess;
        gl_TessLevelOuter[2] = level_tess;
    }

    gl_out[gl_InvocationID].gl_Position =gl_in[gl_InvocationID].gl_Position;
    // Pass by vertex information
    tc_position[gl_InvocationID] = v_position[gl_InvocationID];
    tc_texCoord[gl_InvocationID] = v_texCoord[gl_InvocationID];
}