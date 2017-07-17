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

std::unique_ptr<const Glyph[]> TextRasterizer::rasterize(const std::string &text, const int x, const int y, const int windowWidth, const int windowHeight, float &width, float &height, unsigned int &glyphCount) const {
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

    std::unique_ptr<Glyph[]> glyphs = std::make_unique<Glyph[]>(glyphCount);

    int maxX = 0;

    int cx = x;
    int cy = y;

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

        glyphs[i].textureWidth = pow(2, ceil(log(ftBitmap.width) / log(2)));
        glyphs[i].textureHeight = pow(2, ceil(log(ftBitmap.rows) / log(2)));
        glyphs[i].textureData = std::make_unique<unsigned char[]>(static_cast<size_t>(glyphs[i].textureWidth * glyphs[i].textureHeight));
        for (unsigned int iy = 0; iy < ftBitmap.rows; iy++) {
            memcpy(glyphs[i].textureData.get() + iy * static_cast<unsigned int>(glyphs[i].textureWidth), ftBitmap.buffer + iy * static_cast<unsigned int>(ftBitmap.width), ftBitmap.width);
        }

        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        const float ya = static_cast<float>(glyphPos[i].y_advance) / 64;
        const float xo = static_cast<float>(glyphPos[i].x_offset) / 64;
        const float yo = static_cast<float>(glyphPos[i].y_offset) / 64;

        glyphs[i].x0 = cx + xo + slot->bitmap_left;
        glyphs[i].y0 = floor(cy + yo + slot->bitmap_top);
        glyphs[i].x1 = glyphs[i].x0 + ftBitmap.width;
        glyphs[i].y1 = floor(glyphs[i].y0 - ftBitmap.rows);

        glyphs[i].s0 = 0.0f;
        glyphs[i].t0 = 0.0f;
        glyphs[i].s1 = static_cast<float>(ftBitmap.width) / glyphs[i].textureWidth;
        glyphs[i].t1 = static_cast<float>(ftBitmap.rows) / glyphs[i].textureHeight;

        if (glyphs[i].x1 >= windowWidth) {
            glyphs[i].x0 -= cx;
            glyphs[i].y0 -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
            glyphs[i].x1 -= cx;
            glyphs[i].y1 -= std::ceil(1.2f * fontSize);
            cx -= cx;
            cy -= std::ceil(1.2f * fontSize);
            maxX = windowWidth;
        }

        cx += xa;
        cy += ya;

        if (cx > maxX) {
            maxX = cx;
        }
    }
    cy -= std::ceil(1.2f * fontSize);

    width = maxX - x;
    height = y - cy;

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
