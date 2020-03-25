#version 430 core

layout (vertices = 3) out;

uniform float level_tess;

void main(void)
{
    if (gl_InvocationID == 0)
    {
        gl_TessLevelInner[0] = level_tess;
        gl_TessLevelOuter[0] = level_tess;
        gl_TessLevelOuter[1] = level_tess;
        gl_TessLevelOuter[2] = level_tess;
    }

    gl_out[gl_InvocationID].gl_Position =
        gl_in[gl_InvocationID].gl_Position;
}