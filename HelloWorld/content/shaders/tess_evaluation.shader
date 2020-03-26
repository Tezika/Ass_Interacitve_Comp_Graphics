#version 430 core

layout (quads, equal_spacing) in;

in vec4 tc_position[];
in vec2 tc_texCoord[];

out vec3 fragPos;
out vec3 lightPos;
out vec2 texCoord;
out vec4 fragPosInLightView;

uniform sampler2D tex_disp;
uniform sampler2D tex_normal;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;
uniform mat4 mat_light;
uniform mat4 mat_lightSpaceTransformation;
uniform vec3 worldPos_light;

uniform int displacement = 0;

void main(void)
{
    vec4 p1 = mix(tc_position[0], tc_position[1], gl_TessCoord.x);
    vec4 p2 = mix(tc_position[2], tc_position[3], gl_TessCoord.x);
    vec4 te_position = mix(p1, p2, gl_TessCoord.y);
    vec2 tc1 = mix(tc_texCoord[0], tc_texCoord[1], gl_TessCoord.x);
    vec2 tc2 = mix(tc_texCoord[2], tc_texCoord[3], gl_TessCoord.x);
    texCoord = mix(tc1, tc2, gl_TessCoord.y);

    if(displacement == 1)
    {
        // calculate normal based on the normal map
	    // obtain normal from normal map in range [0,1]
        vec3 norm = texture(tex_normal, texCoord).rgb;
        // transform normal vector to range [-1,1]
	    norm = normalize(norm * 2.0 - 1.0);

        // Generate new vertices based on displacement map.
        vec3 dispData = texture(tex_disp, texCoord).rgb;
        float disp = 2 * (dispData.x + dispData.y + dispData.z);
        te_position = vec4(te_position.xyz + norm * disp, 1);

	    vec4 fragPos4 = mat_view * mat_model * te_position;
	    fragPos = vec3(fragPos4)/fragPos4.w;
	    vec4 lightPos4 = mat_view * mat_light * vec4(worldPos_light, 1);
	    lightPos = vec3(lightPos4)/lightPos4.w;
    }
    fragPosInLightView = mat_lightSpaceTransformation * mat_model * te_position; 
    gl_Position = mat_projection * mat_view * mat_model * te_position;
}