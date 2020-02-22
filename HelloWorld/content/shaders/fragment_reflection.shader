#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
in vec3 normalInterp;
in vec3 vertexPos;
out vec4 o_color;

// For cubemap
uniform samplerCube tex_cubemap;

// Entry Point
//============

void main()
{
	vec3 viewDir = normalize(-vertexPos);
    vec3 r = reflect(viewDir, normalize(normalInterp));
    o_color = vec4(texture(tex_cubemap, r).rgb, 1.0);
}