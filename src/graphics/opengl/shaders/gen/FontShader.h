#ifndef FONTSHADER_H
#define FONTSHADER_H

const char *fontVertexShaderSource =
	"#version 400\n"
	"uniform mat4 transform;\n"
	"layout (location = 0) in vec3 vertexIn;\n"
	"layout (location = 1) in vec2 texCoordIn;\n"
	"\n"
	"out vec2 texCoordPipe;\n"
	"void main() {\n"
	"    gl_Position = transform * vec4(vertexIn, 1.0);\n"
	"    texCoordPipe = texCoordIn;\n"
	"}";

const char *fontFragmentShaderSource =
	"#version 400\n"
	"uniform sampler2D textureIn;\n"
	"in vec2 texCoordPipe;\n"
	"out vec4 colorOut;\n"
	"void main() {\n"
	"    colorOut = vec4(0, 0, 0, texture(textureIn, texCoordPipe).r);\n"
	"}";

#endif
