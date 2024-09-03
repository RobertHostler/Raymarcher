#version 330 core

layout (location = 0) in vec3 positionAttribute;
layout (location = 1) in vec3 normalAttribute;
layout (location = 2) in vec3 tangentAttribute;
layout (location = 3) in vec2 textureUV;

out vec2 UV;

void main(){
    gl_Position = vec4(positionAttribute.x, positionAttribute.z, 0.0, 1.0);
    UV = textureUV;
}