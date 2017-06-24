#!/bin/bash

#Build Shaders
echo -ne "#ifndef FONTSHADER_H\n#define FONTSHADER_H\n\nconst char *textureVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/TextureShader.h;
cat src/graphics/opengl/shaders/TextureShader.vert | awk '{if ($0!="}") {print "\t\""$0"\\n\""} else {print "\t\""$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
echo -ne "const char *textureFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;
cat src/graphics/opengl/shaders/TextureShader.frag | awk '{if ($0!="}") {print "\t\""$0"\\n\""} else {print "\t\""$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
echo -ne "#endif\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;

echo -ne "#ifndef TEXTURESHADER_H\n#define TEXTURESHADER_H\n\nconst char *fontVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/FontShader.h;
cat src/graphics/opengl/shaders/FontShader.vert | awk '{if ($0!="}") {print "\t\""$0"\\n\""} else {print "\t\""$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
echo -ne "const char *fontFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/FontShader.h;
cat src/graphics/opengl/shaders/FontShader.frag | awk '{if ($0!="}") {print "\t\""$0"\\n\""} else {print "\t\""$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
echo -ne "#endif\n" >> src/graphics/opengl/shaders/gen/FontShader.h;

#Compile & Run Netrunner
g++ -o netrunner\
    -lglfw -lGL -lGLEW -lfreetype -lharfbuzz\
    -I /usr/include/freetype2 -I /usr/include/harfbuzz\
    src/graphics/opengl/Text.cpp src/graphics/opengl/Box.cpp src/graphics/opengl/Window.cpp\
    src/graphics/text/TextRasterizer.cpp\
    src/html/HTMLParser.cpp src/html/TextNode.cpp src/html/TagNode.cpp src/html/Node.cpp\
    src/networking/HTTPResponse.cpp src/networking/HTTPRequest.cpp\
    src/main.cpp\
&& ./netrunner http://gyroninja.net/a.html;
