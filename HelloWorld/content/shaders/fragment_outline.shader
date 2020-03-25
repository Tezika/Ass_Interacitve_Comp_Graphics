#version 330 core
// Output
//=======

// Whatever color value is output from the fragment shader
// will determine the color of the corresponding pixel on the screen
out vec4 o_color;

void main()
{    
    o_color = vec4(1.0, 1.0, 0.0, 1.0);         
}  