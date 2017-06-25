#ifndef FONTSHADER_H
#define FONTSHADER_H

const char *fontVertexShaderSource =
	"#version 400\n"
	"layout (location = 0) in vec3 vertexIn;\n"
	"layout (location = 1) in vec2 texCoordIn;\n"
	"out vec2 texCoordPipe;\n"
	"void main() {\n"
	"    gl_Position = vec4(vertexIn, 1.0);\n"
	"    texCoordPipe = texCoordIn;\n"
	"}";

const char *fontFragmentShaderSource =
	"#version 400\n"
	"out vec4 colorOut;\n"
	"in vec2 texCoordPipe;\n"
	"uniform sampler2D textureIn;\n"
	"void main() {\n"
	"    colorOut = vec4(0, 0, 0, texture(textureIn, texCoordPipe).r);\n"
	"}";

#endif
