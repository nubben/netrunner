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
private:
    int fontSize;
public:
    TextRasterizer(const std::string &fontPath, const int size, const unsigned int resolution, const bool bold);
    ~TextRasterizer();
    std::unique_ptr<Glyph[]> rasterize(const std::string &text, const int x, const int y, const int windowWidth, const int windowHeight, float &height, unsigned int &glyphCount) const;
    bool isUnicodeBMP(const FT_Face &face) const;
    FT_Library lib;
    hb_font_t *font;
    hb_buffer_t *buffer;
    std::unique_ptr<FT_Face> face;
};

#endif