#version 400
uniform mat4 transform;
layout (location = 0) in vec3 vertexIn;
layout (location = 1) in vec4 colorIn;
layout (location = 2) in vec2 texCoordIn;

out vec4 colorPipe;
out vec2 texCoordPipe;
void main() {
    gl_Position = transform * vec4(vertexIn, 1.0);
    colorPipe = colorIn;
    texCoordPipe = texCoordIn;
}
