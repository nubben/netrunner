#include "TextRasterizer.h"
#include <cmath>
#include <limits>
#include <iostream>

TextRasterizer::TextRasterizer(const std::string &fontPath, int size, int resolution) {
    FT_Init_FreeType(&lib);

    face = std::make_unique<FT_Face>();
    if (FT_New_Face(lib, fontPath.c_str(), 0, face.get())) {
        std::cout << "Could not open font" << std::endl;
        return;
    }
    if (!isUnicodeBMP(*face)) {
        std::cout << "Font is not Unicode BMP" << std::endl;
        return;
    }
    if (FT_Set_Char_Size(*face, 0, size * 64, resolution, resolution)) {
        std::cout << "Could not set font size" << std::endl;
        return;
    }

    font = hb_ft_font_create(*face, NULL);

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

std::unique_ptr<const Glyph[]> TextRasterizer::rasterize(const std::string &text, const int x, const int y, unsigned int &glyphCount) const {
    hb_buffer_reset(buffer);
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));

    hb_buffer_add_utf8(buffer, text.c_str(), text.length(), 0, text.length());

    const hb_feature_t KerningOn = { HB_TAG('k', 'e', 'r', 'n'), 1, 0, std::numeric_limits<unsigned int>::max() };
    hb_shape(font, buffer, &KerningOn, 1);

    const hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    const hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);

    std::unique_ptr<Glyph[]> glyphs = std::unique_ptr<Glyph[]>(new Glyph[glyphCount]);

    int cx = x;
    int cy = y;

    for (int i = 0; i < glyphCount; i++) {
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
        glyphs[i].textureData = std::unique_ptr<unsigned char[]>(new unsigned char[glyphs[i].textureWidth * glyphs[i].textureHeight]);
        for (int iy = 0; iy < ftBitmap.rows; iy++) {
            memcpy(glyphs[i].textureData.get() + iy * glyphs[i].textureWidth, ftBitmap.buffer + iy * ftBitmap.width, ftBitmap.width);
        }

        const float xa = (float) glyphPos[i].x_advance / 64;
        const float ya = (float) glyphPos[i].y_advance / 64;
        const float xo = (float) glyphPos[i].x_offset / 64;
        const float yo = (float) glyphPos[i].y_offset / 64;

        glyphs[i].x0 = cx + xo + slot->bitmap_left;
        glyphs[i].y0 = floor(cy + yo + slot->bitmap_top);
        glyphs[i].x1 = glyphs[i].x0 + ftBitmap.width;
        glyphs[i].y1 = floor(glyphs[i].y0 - ftBitmap.rows);

        glyphs[i].s0 = 0.0f;
        glyphs[i].t0 = 0.0f;
        glyphs[i].s1 = (float) ftBitmap.width / glyphs[i].textureWidth;
        glyphs[i].t1 = (float) ftBitmap.rows / glyphs[i].textureHeight;

        cx += xa;
        cx += ya;
    }

    return glyphs;
}

const bool TextRasterizer::isUnicodeBMP(const FT_Face &face) const {
    for (int i = 0; i < face->num_charmaps; i++) {
        if (((face->charmaps[i]->platform_id == 0) && (face->charmaps[i]->encoding_id == 3)) || ((face->charmaps[i]->platform_id == 3) && (face->charmaps[i]->encoding_id == 1))) {
            FT_Set_Charmap(face, face->charmaps[i]);
            return true;
        }
    }
    return false;
}
