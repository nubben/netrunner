#include "TextRasterizer.h"
#include <cmath>
#include <limits>
#include <iostream>

TextRasterizer::TextRasterizer(const std::string &fontPath, const int size, const unsigned int resolution, const bool bold) {
    fontSize = size;
    FT_Init_FreeType(&lib);

    face = std::make_unique<FT_Face>();
    if (FT_New_Face(lib, bold ? (fontPath.substr(0, fontPath.length() - 4) + "-Bold.ttf").c_str() : fontPath.c_str(), 0, face.get())) {
        std::cout << "Could not open font" << std::endl;
        return;
    }
    if (!isUnicodeBMP(*face)) {
        std::cout << "Font is not Unicode BMP" << std::endl;
        return;
    }
    if (FT_Set_Char_Size(*face, 0, fontSize * 64, resolution, resolution)) {
        std::cout << "Could not set font size" << std::endl;
        return;
    }

    font = hb_ft_font_create(*face, nullptr);

    buffer = hb_buffer_create();
    if (!hb_buffer_allocation_successful(buffer)) {
        std::cout << "Could not create HarfBuzz buffer" << std::endl;
        return;
    }
}

TextRasterizer::~TextRasterizer() {
    hb_buffer_destroy(buffer);
    hb_font_destroy(font);
    FT_Done_FreeType(lib);
}

// we don't need x,y here except we do need to know how much width we have available
std::unique_ptr<Glyph[]> TextRasterizer::rasterize(const std::string &text, const int x, const int y, const int windowWidth, const int windowHeight, float &width, float &height, unsigned int &glyphCount) const {
    //std::cout << "rasterizing [" << text << "] at " << x << "x" << y << " window:" << windowWidth << "x" << windowHeight << std::endl;
    if (x>windowWidth) {
        std::cout << "TextRasterizer::rasterize - x [" << static_cast<int>(x) << "] requested outside of window width [" << static_cast<int>(windowWidth)<< "] for text[" << text << "]" << std::endl;
        return nullptr;
    }
    hb_buffer_reset(buffer);
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));

    hb_buffer_add_utf8(buffer, text.c_str(), text.length(), 0, text.length());

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wold-style-cast"
    const hb_feature_t KerningOn = { HB_TAG('k', 'e', 'r', 'n'), 1, 0, std::numeric_limits<unsigned int>::max() };
    #pragma GCC diagnostic pop
    hb_shape(font, buffer, &KerningOn, 1);

    const hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    const hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);

    //std::cout << "at:" << x << "x" << y << std::endl;

    // figure out width/height
    int cx = 0;
    int cy = 0;
    int xmax = 0;
    int y0max = 0, y1max = 0;
    int lines = 1;
    for (unsigned int i = 0; i < glyphCount; i++) {
        if (FT_Load_Glyph(*face, glyphInfo[i].codepoint, FT_LOAD_DEFAULT)) {
            std::cout << "Could not load glyph" << std::endl;
            return nullptr;
        }
        const FT_GlyphSlot slot = (*face)->glyph;

        if (FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) {
            std::cout << "Could not render glyph" << std::endl;
            return nullptr;
        }
        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        const float ya = static_cast<float>(glyphPos[i].y_advance) / 64; //mostly 0s

        if (cx + x >= windowWidth) {
            //std::cout << "multine text: [" << text << "] new line:" << cy << std::endl;
            xmax = windowWidth - x; // wrap to the beginning of the element
            cx -= xmax;
            cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
            lines++;
        }

        //std::cout << "glyph:" << xa << "x" << ya << std::endl;

        cx += xa;
        cy += ya; // is normal for y0 at bottom

        const FT_Bitmap ftBitmap = slot->bitmap;

        const float yo = static_cast<float>(glyphPos[i].y_offset) / 64;
        int y0 = static_cast<int>(floor(yo + slot->bitmap_top));
        int y1 = y0 + static_cast<int>(ftBitmap.rows);
        y0max=std::max(y0max, y0);
        y1max=std::max(y1max, y1);
        xmax=std::max(xmax, cx);

    }
    // at least one line
    cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
    height = -cy;
    width = xmax;
    y1max *= lines;
    //std::cout << "now:" << width << "x" << height << std::endl;
    if (height<y1max) {
        height=y1max;
    }
    if (xmax==windowWidth - x) {
        std::cout << "Wrapped text[" << text << "] over " << lines << " lines " << xmax << "x" << static_cast<int>(height) << std::endl;
    }

    std::unique_ptr<Glyph[]> glyphs = std::make_unique<Glyph[]>(1);
    Glyph *line = &glyphs[0]; // lazy alias
    line->textureWidth = pow(2, ceil(log(width) / log(2)));
    line->textureHeight = pow(2, ceil(log(height) / log(2)));
    //std::cout << "text texture size:" << line->textureWidth << "x" << line->textureHeight << std::endl;
    line->textureData = std::make_unique<unsigned char[]>(static_cast<size_t>(line->textureWidth * line->textureHeight));
    if (!line->textureData) {
        std::cout << "failed to create texture" << static_cast<int>(width) << "X" << static_cast<int>(height) << std::endl;
        return nullptr;
    }

    // translation information
    line->x0 = x; // wrap to element start
    line->y0 = y;
    line->x1 = x + width;
    line->y1 = y - height;
    //std::cout << "xd: " << static_cast<int>(line->x1-line->x0) << " yd: " << static_cast<int>(line->y0-line->y1) << std::endl;

    // texture coords
    line->s0 = 0.0f;
    line->t0 = 0.0f;
    line->s1 = width / line->textureWidth;
    line->t1 = height / line->textureHeight;
    //std::cout << "s1: " << line->s1 << " t1: " << line->t1 << std::endl;

    // copy all glyphs into one single glyph
    cx = 0; // reset
    cy = 0;
    for (unsigned int i = 0; i < glyphCount; i++) {
        if (FT_Load_Glyph(*face, glyphInfo[i].codepoint, FT_LOAD_DEFAULT)) {
            std::cout << "Could not load glyph" << std::endl;
            return nullptr;
        }
        const FT_GlyphSlot slot = (*face)->glyph;

        if (FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL)) {
            std::cout << "Could not render glyph" << std::endl;
            return nullptr;
        }

        const FT_Bitmap ftBitmap = slot->bitmap;

        // figure out glyph starting point
        const float yo = static_cast<float>(glyphPos[i].y_offset) / 64;
        int y0 = static_cast<int>(floor(yo + slot->bitmap_top));

        int bump = 0; // Y adjust for this glyph
        if (y0) {
            // (25-36)-25 = 0-11
            // 36-(25-36) = 11-0
            bump = y0max - y0;
        }
        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        // if this char is too width for this line, advance to next line
        if (x + xa + cx >= windowWidth) {
            cx = 0;
            cy += std::ceil(1.2f * fontSize);// 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
            //std::cout << "textWrap - cx reset to: " <<  cx << " cy is now: " << cy << std::endl;
        }
        //std::cout << "placing glyph[" << text[i] << "] at " <<  cx << " cy is now: " << cy << std::endl;
        for (unsigned int iy = 0; iy < ftBitmap.rows; iy++) {
            // source is 0 to (0:iy:rows)
            // dest is cx+bl, (0:iy:rows)+(0:cy:height)+bump
            //std::cout << "placing glyph row at " << (cx + slot->bitmap_left) << "x" << ((iy + cy) + bump) << std::endl;
            memcpy(line->textureData.get() + (cx + slot->bitmap_left) + ((iy + static_cast<unsigned int>(cy)) + static_cast<unsigned int>(bump)) * static_cast<unsigned int>(line->textureWidth), ftBitmap.buffer + iy * static_cast<unsigned int>(ftBitmap.width), ftBitmap.width);
        }

        cx += xa;
    }
    // make one single glyph
    glyphCount=1;
    //std::cout << "final size: " << (int)width << "x" << (int)height << std::endl;
    //std::cout << "at: " << (int)line->x0 << "x" << (int)line->y0 << " to: " << (int)line->x1 << "x" << (int)line->y1 <<std::endl;

    return glyphs;
}

bool TextRasterizer::isUnicodeBMP(const FT_Face &ftFace) const {
    for (int i = 0; i < ftFace->num_charmaps; i++) {
        if (((ftFace->charmaps[i]->platform_id == 0) && (ftFace->charmaps[i]->encoding_id == 3)) || ((ftFace->charmaps[i]->platform_id == 3) && (ftFace->charmaps[i]->encoding_id == 1))) {
            FT_Set_Charmap(ftFace, ftFace->charmaps[i]);
            return true;
        }
    }
    return false;
}
