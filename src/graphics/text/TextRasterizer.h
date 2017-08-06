#ifndef TEXTRASTERIZER_H
#define TEXTRASTERIZER_H

#include "hb.h"
#include "hb-ft.h"
#include <memory>
#include <string>
#include <algorithm>

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

// we don't need x,y here except we do need to know how much width we have available
// we need to know x and y it left off at
// x in case we line wrap, so we know where we left off for inline
// y in case of multiline wrap, where the last line ended for inline
// and we'll need to know the starting x
// we don't need the starting y tbh
// even if windowWidth isn't a windowWidth, we still need some type of point to wrap on
struct rasterizationRequest {
    std::string text;
    int availableWidth; // (relative to 0)
    // not availableHeight because we're either going to wrap or not
    int startX; // starting x point (relative to 0) this is currently
    //int wrapToX; // next line starts at X (was always 0)

    int sourceStartX = 0; // start reading text source at and place at destination 0
    int sourceStartY = 0;
    bool noWrap = false; // different than overflow but related
};

struct rasterizationResponse {
    float width;
    float height;
    int glyphCount;
    int endingX;
    int endingY;
    bool wrapped;
    float x0;
    float y0;
    float x1;
    float y1; // calculated on other members here
    float s0;
    float t0;
    float s1; // calculated on other members here
    float t1; // calculated on other members here
    int textureWidth;
    int textureHeight;
    std::unique_ptr<unsigned char[]> textureData;
};

// container for a font at size bold/regular
class TextRasterizer {
private:
    int fontSize;
public:
    TextRasterizer(const std::string &fontPath, const int size, const unsigned int resolution, const bool bold);
    ~TextRasterizer();
    //std::unique_ptr<Glyph[]> rasterize(const std::string &text, const int x, const int windowWidth, const int wrapToX, float &width, float &height, unsigned int &glyphCount, int &endingX, int &endingY, bool &wrapped) const;
    std::unique_ptr<std::tuple<int, int>> size(const rasterizationRequest &request) const;
    std::unique_ptr<rasterizationResponse> rasterize(const rasterizationRequest &request) const;
    bool isUnicodeBMP(const FT_Face &face) const;
    FT_Library lib;
    hb_font_t *font;
    hb_buffer_t *buffer;
    std::unique_ptr<FT_Face> face;
};

#endif
