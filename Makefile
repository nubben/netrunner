UNAME := $(shell uname)
CXX        = g++

GIT_VERSION := $(shell git describe --abbrev=4 --dirty --always --tags)

CXXFLAGS   = -O3 -flto=8 -std=c++14 -DVERSION=\"$(GIT_VERSION)\"
WARNINGS   = -Werror -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused -Wzero-as-null-pointer-constant -Wuseless-cast
LIBS       = -lglfw -lGL -lGLEW -lfreetype -lharfbuzz
LDFLAGS    = -O3 -flto=8
INCPATH    = -I /usr/include/freetype2 -I /usr/include/harfbuzz

ifeq ($(UNAME), Darwin)
CXXFLAGS   = -O3 -std=c++1y -DVERSION=\"$(GIT_VERSION)\"
WARNINGS   =
LIBS       = -L/usr/local/lib -lglfw3 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo -lGLEW -lfreetype -lharfbuzz
LDFLAGS    = -O3
INCPATH    = -I /usr/local/include -I /usr/local/include/freetype2 -I /usr/local/include/harfbuzz
endif

EXECUTABLE = netrunner
LINK       = g++

SRCDIR     = src
OBJDIR     = gen
DEPDIR     = d

SOURCES = $(subst ./,,$(shell find . -name *.cpp))
OBJECTS = $(subst $(SRCDIR),$(OBJDIR),$(SOURCES:.cpp=.o))

all: $(SOURCES) netrunner

netrunner: $(OBJECTS)
	$(LINK) $(LDFLAGS) -o $@ $^ $(LIBS)

# make make 3.81 happy (since it can't parse any echo parameters)
ifeq ($(UNAME), Darwin)
shaders:
ifneq ($(shell cat src/graphics/opengl/shaders/FontShader.vert src/graphics/opengl/shaders/FontShader.frag src/graphics/opengl/shaders/TextureShader.vert src/graphics/opengl/shaders/TextureShader.frag | md5sum), $(shell cat src/graphics/opengl/shaders/gen/hashsum))
	@mkdir -p "src/graphics/opengl/shaders/gen"
	@echo "#ifndef FONTSHADER_H\n#define FONTSHADER_H\n\nconst char *fontVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/FontShader.h;
	@cat src/graphics/opengl/shaders/FontShader.vert | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
	@echo "const char *fontFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/FontShader.h;
	@cat src/graphics/opengl/shaders/FontShader.frag | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
	@echo "#endif\n" >> src/graphics/opengl/shaders/gen/FontShader.h;
	@echo "#ifndef TEXTURESHADER_H\n#define TEXTURESHADER_H\n\nconst char *textureVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/TextureShader.h;
	@cat src/graphics/opengl/shaders/TextureShader.vert | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@echo "const char *textureFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@cat src/graphics/opengl/shaders/TextureShader.frag | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@echo "#endif\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@cat src/graphics/opengl/shaders/FontShader.vert src/graphics/opengl/shaders/FontShader.frag src/graphics/opengl/shaders/TextureShader.vert src/graphics/opengl/shaders/TextureShader.frag | md5sum > src/graphics/opengl/shaders/gen/hashsum;
endif
else
shaders:
ifneq ($(shell cat src/graphics/opengl/shaders/FontShader.vert src/graphics/opengl/shaders/FontShader.frag src/graphics/opengl/shaders/TextureShader.vert src/graphics/opengl/shaders/TextureShader.frag | md5sum), $(shell cat src/graphics/opengl/shaders/gen/hashsum))
	@mkdir -p "src/graphics/opengl/shaders/gen"
	@cat src/graphics/opengl/shaders/FontShader.vert src/graphics/opengl/shaders/FontShader.frag src/graphics/opengl/shaders/TextureShader.vert src/graphics/opengl/shaders/TextureShader.frag | md5sum > src/graphics/opengl/shaders/gen/hashsum;
	@/bin/echo -ne "#ifndef FONTSHADER_H\n#define FONTSHADER_H\n\nconst char *fontVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/FontShader.h;
	@cat src/graphics/opengl/shaders/FontShader.vert | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
	@/bin/echo -ne "const char *fontFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/FontShader.h;
	@cat src/graphics/opengl/shaders/FontShader.frag | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/FontShader.h;
	@/bin/echo -ne "#endif\n" >> src/graphics/opengl/shaders/gen/FontShader.h;
	@/bin/echo -ne "#ifndef TEXTURESHADER_H\n#define TEXTURESHADER_H\n\nconst char *textureVertexShaderSource =\n" > src/graphics/opengl/shaders/gen/TextureShader.h;
	@cat src/graphics/opengl/shaders/TextureShader.vert | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@/bin/echo -ne "const char *textureFragmentShaderSource =\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@cat src/graphics/opengl/shaders/TextureShader.frag | awk '{if ($$0!="}") {print "\t\""$$0"\\n\""} else {print "\t\""$$0"\";\n"}}' >> src/graphics/opengl/shaders/gen/TextureShader.h;
	@/bin/echo -ne "#endif\n" >> src/graphics/opengl/shaders/gen/TextureShader.h;
endif
endif

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | shaders
	@mkdir -p $(@D)
	@mkdir -p $(subst gen,d,$(@D))
	$(CXX) -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td $(CXXFLAGS) $(INCPATH) $(WARNINGS) -c -o $@ $<
	@mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

$(DEPDIR)/%d: ;
.PRECIOUS: $(DEPDIR)/%.d

clean:
	-@rm -rf src/graphics/opengl/shaders/gen $(OBJDIR) $(EXECUTABLE) 2>/dev/null || true

include $(addsuffix .d,$(subst $(SRCDIR),$(DEPDIR),$(basename $(SOURCES))))
