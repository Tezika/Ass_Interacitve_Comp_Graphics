#version 430 core

in vec4 fragPosInLightSpace;
// Ouput data
layout(location = 0) out float fragmentdepth;

void main(){
    // Not really needed, OpenGL does it anyway
    fragmentdepth = fragPosInLightSpace.z;
}