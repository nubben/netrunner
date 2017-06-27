#version 400
uniform sampler2D textureIn;
in vec2 texCoordPipe;
out vec4 colorOut;
void main() {
    colorOut = vec4(0, 0, 0, texture(textureIn, texCoordPipe).r);
}
