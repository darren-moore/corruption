#version 330 core

layout(location = 0) in vec4 inPosition;
layout (location = 1) in vec2 inTexCoord;

uniform mat4 projection;

out vec2 texCoord;

void main() {
    gl_Position = projection * vec4(inPosition.xy, 0, 1);
    texCoord = inTexCoord.xy;
}