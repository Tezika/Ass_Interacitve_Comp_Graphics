#version 430 core

layout (triangles) in;

in vec4 tc_position[];
in vec2 tc_texCoord[];

out vec3 fragPos;
out vec3 normal;
out vec3 lightPos;

uniform sampler2D tex_disp;
uniform sampler2D tex_normal;

uniform mat4 mat_model;
uniform mat4 mat_view;
uniform mat4 mat_projection;
uniform mat4 mat_light;
uniform vec3 worldPos_light;

void main(void)
{
    vec4 te_position = (gl_TessCoord.x * tc_position[0]) + (gl_TessCoord.y * tc_position[1]) +(gl_TessCoord.z * tc_position[2]);
    gl_Position = mat_projection * mat_view * mat_model * te_position;
    vec2 te_texCoord =  (gl_TessCoord.x * tc_texCoord[0] + gl_TessCoord.y * tc_texCoord[1] + gl_TessCoord.z * tc_texCoord[2]);

    // // calculate normal based on the normal map
	// // obtain normal from normal map in range [0,1]
	// vec2 texCoord = te_texCoord[i];
    // vec3 normal = texture(tex_normal, texCoord).rgb;
    // // transform normal vector to range [-1,1]
    // normal = normalize(normal * 2.0 - 1.0);

	// // Generate new vertices based on displacement map.
	// float dispData = texture(tex_disp, texCoord).r;

	// // float height = 0.33*dispData.x + 0.33*dispData.y + 0.33*dispData.z;
    // vec4 newPos = vec4(te_position[i].xyz + normal * dispData, 1);

	// vec4 fragPos4 = mat_view * mat_model * newPos;
	// fragPos = vec3(fragPos4)/fragPos4.w;
	// vec4 lightPos4 = mat_view * mat_light * vec4(worldPos_light, 1);
	// lightPos = vec3(lightPos4)/lightPos4.w;
	
    // gl_Position = mat_projection * mat_view * mat_model * newPos;
}