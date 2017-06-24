#ifndef TEXTRASTERIZER_H
#define TEXTRASTERIZER_H

#include "hb.h"
#include "hb-ft.h"
#include <memory>
#include <string>

struct Glyph {
    float x0;
    float y0;
    float x1;
    float y1;
    float s0;
    float t0;
    float s1;
    float t1;
    int textureWidth;
    int textureHeight;
    std::unique_ptr<unsigned char[]> textureData;
};

class TextRasterizer {
public:
    TextRasterizer(std::string font, int size, int resolution);
    ~TextRasterizer();
    std::unique_ptr<Glyph[]> rasterize(std::string text, int x, int y, unsigned int &glyphCount);
    bool isUnicodeBMP(FT_Face face);
    FT_Library lib;
    hb_font_t *font;
    hb_buffer_t *buffer;
    std::unique_ptr<FT_Face> face;
};

#endif