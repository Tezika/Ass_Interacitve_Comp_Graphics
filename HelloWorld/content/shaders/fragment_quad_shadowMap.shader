#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in  vec2 texCoord;
out vec4 o_color;

// For renderTex
uniform sampler2D tex_render;
uniform float near_plane;
uniform float far_plane;
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{             
    float depthValue = texture(tex_render, texCoord).r;
    o_color = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective
}  