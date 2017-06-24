#version 400
out vec4 colorOut;
in vec2 texCoordPipe;
uniform sampler2D textureIn;
void main() {
    colorOut = texture(textureIn, texCoordPipe);
}
