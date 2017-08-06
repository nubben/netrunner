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

std::unique_ptr<std::tuple<int, int>> TextRasterizer::size(const rasterizationRequest &request) const {
    
    if (request.startX == request.availableWidth) {
        std::cout << "TextRasterizer::size - x [" << static_cast<int>(request.startX) << "] matches window width [" << static_cast<int>(request.availableWidth)<< "] for text[" << request.text << "] no room to render anything" << std::endl;
        return nullptr;
        
    }
    if (request.startX > request.availableWidth) {
        std::cout << "TextRasterizer::size - x [" << static_cast<int>(request.startX) << "] requested outside of window width [" << static_cast<int>(request.availableWidth)<< "] for text[" << request.text << "]" << std::endl;
        return nullptr;
    }
    
    unsigned int glyphCount;
    
    hb_buffer_reset(buffer);
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));
    
    hb_buffer_add_utf8(buffer, request.text.c_str(), request.text.length(), 0, request.text.length());
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    const hb_feature_t KerningOn = { HB_TAG('k', 'e', 'r', 'n'), 1, 0, std::numeric_limits<unsigned int>::max() };
#pragma GCC diagnostic pop
    hb_shape(font, buffer, &KerningOn, 1);
    
    const hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    const hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);
    
    std::unique_ptr<rasterizationResponse> response = std::make_unique<rasterizationResponse>();
    
    // figure out width/height
    int cx = 0;
    int cy = 0;
    int xmax = 0;
    int y0max = 0, y1max = 0;
    int lines = 1;
    response->wrapped = false;
    int lineXStart = request.startX;
    int leftPadding = 0;
    int wrapToX = 0;
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
        
        // how much space does this character take
        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        const float ya = static_cast<float>(glyphPos[i].y_advance) / 64; //mostly 0s
        
        // do we need to padding the texture to the left for any lines
        if (cx == 0) {
            if (slot->bitmap_left < 0) {
                // figure out max amount of padding we need
                leftPadding = std::max(leftPadding, -slot->bitmap_left);
            }
        }
        
        // wrap to next line on width
        if (cx + lineXStart >= request.availableWidth) {
            response->wrapped = true;
            if (request.noWrap) {
                glyphCount = i;
            } else {
                //std::cout << "multine text: [" << text << "] new line:" << cy << " x: " << (int)x << "+ cx:" << (int)cx << std::endl;
                //std::cout << "line #" << lines << " starts at " << lineXStart << " ends at " << lineXStart + cx << std::endl;
                xmax = request.availableWidth - wrapToX; // the whole width of parent to the edge of windowWidth
                //std::cout << "new width: " << xmax << std::endl;
                cx = wrapToX; // wrapToX (was 0) (was -= xmax)
                cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
                lines++;
                lineXStart = wrapToX; // change where we start
            }
        }
        
        //std::cout << "glyph:" << xa << "x" << ya << std::endl;
        
        // update glyph space allocation
        cx += xa;
        cy += ya; // is normal for y0 at bottom
        
        // update glyph maxes
        const FT_Bitmap ftBitmap = slot->bitmap;
        const float yo = static_cast<float>(glyphPos[i].y_offset) / 64;
        int y0 = static_cast<int>(floor(yo + slot->bitmap_top));
        int y1 = y0 + static_cast<int>(ftBitmap.rows);
        y0max=std::max(y0max, y0);
        y1max=std::max(y1max, y1);
        
        // track new max width
        xmax=std::max(xmax, cx);
        
    }
    if (leftPadding) {
        xmax+=leftPadding; // increase width;
    }
    // at least one line
    cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
    response->height = -cy;
    response->width = xmax;
    //std::cout << "lines: " << lines << " wrapToX: " << wrapToX << " startX: " << x << " xmax: " << xmax << std::endl;
    //std::cout << "y1max: " << y1max << " lines: " << lines << std::endl;
    y1max *= lines;
    //std::cout << "initial:" << (int)width << "x" << (int)height << std::endl;
    if (response->height < y1max) {
        response->height = y1max;
        //std::cout << "adjusted:" << (int)width << "x" << (int)height << std::endl;
    }
    return std::make_unique<std::tuple<int, int>>(std::tuple<int, int>(response->width, response->height));
}

std::unique_ptr<rasterizationResponse> TextRasterizer::rasterize(const rasterizationRequest &request) const {
   
    if (request.startX == request.availableWidth) {
        std::cout << "TextRasterizer::rasterize - x [" << static_cast<int>(request.startX) << "] matches window width [" << static_cast<int>(request.availableWidth)<< "] for text[" << request.text << "] no room to render anything" << std::endl;
        return nullptr;
        
    }
    if (request.startX > request.availableWidth) {
        std::cout << "TextRasterizer::rasterize - x [" << static_cast<int>(request.startX) << "] requested outside of window width [" << static_cast<int>(request.availableWidth)<< "] for text[" << request.text << "]" << std::endl;
        return nullptr;
    }
    
    unsigned int glyphCount;

    hb_buffer_reset(buffer);
    hb_buffer_set_direction(buffer, HB_DIRECTION_LTR);
    hb_buffer_set_language(buffer, hb_language_from_string("en", 2));
    
    hb_buffer_add_utf8(buffer, request.text.c_str(), request.text.length(), 0, request.text.length());
    
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
    const hb_feature_t KerningOn = { HB_TAG('k', 'e', 'r', 'n'), 1, 0, std::numeric_limits<unsigned int>::max() };
#pragma GCC diagnostic pop
    hb_shape(font, buffer, &KerningOn, 1);
    
    const hb_glyph_info_t *glyphInfo = hb_buffer_get_glyph_infos(buffer, &glyphCount);
    const hb_glyph_position_t *glyphPos = hb_buffer_get_glyph_positions(buffer, &glyphCount);
    
    std::unique_ptr<rasterizationResponse> response = std::make_unique<rasterizationResponse>();

    // figure out width/height
    int cx = 0;
    int cy = 0;
    int xmax = 0;
    int y0max = 0, y1max = 0;
    int lines = 1;
    response->wrapped = false;
    int lineXStart = request.startX;
    int leftPadding = 0;
    int wrapToX = 0;
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
        
        // how much space does this character take
        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        const float ya = static_cast<float>(glyphPos[i].y_advance) / 64; //mostly 0s
        
        // do we need to padding the texture to the left for any lines
        if (cx == 0) {
            if (slot->bitmap_left < 0) {
                // figure out max amount of padding we need
                leftPadding = std::max(leftPadding, -slot->bitmap_left);
            }
        }
        
        // wrap to next line on width
        if ((cx + lineXStart) - request.sourceStartX >= request.availableWidth) {
            response->wrapped = true;
            if (request.noWrap) {
                std::cout << "TextRasterizer::rasterize - we've hit wrap & noWrap at " << cx << " + " << lineXStart << " < " << request.availableWidth << std::endl;
                // we can only fit partial of the last char
                std::cout << "TextRasterizer::rasterize - truncating " << glyphCount << " char(s) to " << i << " char(s)" << std::endl;
                glyphCount = i;
                // we're done
                break;
            } else {
                //std::cout << "multine text: [" << text << "] new line:" << cy << " x: " << (int)x << "+ cx:" << (int)cx << std::endl;
                //std::cout << "line #" << lines << " starts at " << lineXStart << " ends at " << lineXStart + cx << std::endl;
                xmax = request.availableWidth - wrapToX; // the whole width of parent to the edge of windowWidth
                //std::cout << "new width: " << xmax << std::endl;
                cx = wrapToX; // wrapToX (was 0) (was -= xmax)
                cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
                lines++;
                lineXStart = wrapToX; // change where we start
            }
        }
        
        //std::cout << "glyph:" << xa << "x" << ya << std::endl;
        
        // update glyph space allocation
        cx += xa;
        cy += ya; // is normal for y0 at bottom
        
        // update glyph maxes
        const FT_Bitmap ftBitmap = slot->bitmap;
        const float yo = static_cast<float>(glyphPos[i].y_offset) / 64;
        int y0 = static_cast<int>(floor(yo + slot->bitmap_top));
        int y1 = y0 + static_cast<int>(ftBitmap.rows);
        y0max=std::max(y0max, y0);
        y1max=std::max(y1max, y1);
        
        // track new max width
        xmax=std::max(xmax, cx);
        
    }
    //std::cout << "TextRasterizer::rasterize - xmax exits loop at " << xmax << std::endl;
    if (leftPadding) {
        //std::cout << "TextRasterizer::rasterize - leftPadding " << leftPadding << std::endl;
        xmax += leftPadding; // increase width;
    }
    // at least one line
    cy -= std::ceil(1.2f * fontSize); // 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
    
    //std::cout << "TextRasterizer::rasterize - initial width " << xmax << std::endl;
    response->width = xmax;
    response->height = -cy;
    //std::cout << "lines: " << lines << " wrapToX: " << wrapToX << " startX: " << x << " xmax: " << xmax << std::endl;
    //std::cout << "y1max: " << y1max << " lines: " << lines << std::endl;
    y1max *= lines;
    //std::cout << "initial:" << (int)width << "x" << (int)height << std::endl;
    if (response->height < y1max) {
        response->height = y1max;
        //std::cout << "adjusted:" << (int)width << "x" << (int)height << std::endl;
    }
    // adjust sourceStart
    //std::cout << "TextRasterizer::rasterize - adjust sourceStartX " << request.sourceStartX << std::endl;
    response->width -= request.sourceStartX;
    response->height -= request.sourceStartY;
    //std::cout << "TextRasterizer::rasterize - final:" << static_cast<int>(response->width) << "x" << static_cast<int>(response->height) << std::endl;

    /*
     if (xmax==windowWidth - x) {
     std::cout << "Wrapped text[" << text << "] over " << lines << " lines " << xmax << "x" << static_cast<int>(height) << std::endl;
     }
     */
    //std::cout << "text size: " << (int)width << "x" << (int)height << std::endl;
    
    response->textureWidth = pow(2, ceil(log(response->width) / log(2)));
    response->textureHeight = pow(2, ceil(log(response->height) / log(2)));
    //std::cout << "text texture size:" << line->textureWidth << "x" << line->textureHeight << std::endl;
    size_t size = static_cast<size_t>(response->textureWidth * response->textureHeight); // here?
    response->textureData = std::make_unique<unsigned char[]>(size); // here?
    if (!response->textureData) {
        std::cout << "failed to create texture" << static_cast<int>(response->width) << "X" << static_cast<int>(response->height) << std::endl;
        return nullptr;
    }
    
    // translation information
    response->x0 = -leftPadding; // wrap to element start (wrapToX (0) or x)
    response->y0 = 0;
    response->x1 = -leftPadding + response->width;
    response->y1 = -response->height;
    //std::cout << "xd: " << static_cast<int>(line->x1-line->x0) << " yd: " << static_cast<int>(line->y0-line->y1) << std::endl;
    
    // texture coords
    response->s0 = 0.0f;
    response->t0 = 0.0f;
    response->s1 = response->width / response->textureWidth;
    response->t1 = response->height / response->textureHeight;
    //std::cout << "s1: " << line->s1 << " t1: " << line->t1 << std::endl;

    // copy all glyphs into one single glyph
    // still neds to start at X
    //std::cout << "starting at: " << x << std::endl;
    cx = response->wrapped ? request.startX : 0; // reset
    cy = 0;
    //int miny0 = 99;
    int maxy0 = 0;
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
        
        //miny0 = std::min(y0, miny0);
        maxy0 = std::max(y0, maxy0);
        
        int bump = y0max - y0; // Y adjust for this glyph
        const float xa = static_cast<float>(glyphPos[i].x_advance) / 64;
        // if this char is too width for this line, advance to next line
        if (!request.noWrap && cx + xa >= request.availableWidth) {
            cx = wrapToX;
            cy += std::ceil(1.2f * fontSize);// 1.2 scalar from https://developer.mozilla.org/en-US/docs/Web/CSS/line-height
            //std::cout << "textWrap - cx reset to: " <<  cx << " cy is now: " << cy << std::endl;
        }
        //std::cout << "placing glyph[" << text[i] << "] at " <<  cx << " cy is now: " << cy << std::endl;
        
        if (cx < request.sourceStartX) {
            // skip ahead
            cx += xa;
            continue;
        }
        
        // place glyph bitmap data into texture
        for (unsigned int iy = 0; iy < ftBitmap.rows; iy++) { // line by line
            // source is 0 to (0:iy:rows)
            // dest is cx+bl, (0:iy:rows)+(0:cy:height)+bump
            //std::cout << "placing glyph row at " << (cx + slot->bitmap_left) << "x" << ((iy + cy) + bump) << std::endl;
            unsigned int destPos = (cx - request.sourceStartX + leftPadding + slot->bitmap_left) + ((iy + static_cast<unsigned int>(cy - request.sourceStartY)) + static_cast<unsigned int>(bump)) * static_cast<unsigned int>(response->textureWidth);
            if (destPos >= size) {
                // we're done with this line
                continue;
            }
            unsigned char *src = ftBitmap.buffer + iy * static_cast<unsigned int>(ftBitmap.width);
            unsigned char *dest = response->textureData.get() + destPos;
            for(unsigned int ix = 0; ix < ftBitmap.width; ix++) { // pixel by pixel
                //std::cout << "putting:" << (int)src[ix] << " over " << (int)dest[ix] << std::endl;
                if (destPos + ix >= size) {
                    // we're done with this line
                    continue;
                }
                if (src[ix] && src[ix]>dest[ix]) {
                    dest[ix]=src[ix];
                }
            }
        }
        
        cx += xa;
    }
    //std::cout << "final size: " << (int)width << "x" << (int)height << std::endl;
    //std::cout << "at: " << (int)line->x0 << "x" << (int)line->y0 << " to: " << (int)line->x1 << "x" << (int)line->y1 <<std::endl;
    response->endingX = cx - request.sourceStartX; // maybe should be one xa less?
    //std::ceil(0.5 * 1.2f * fontSize)+2
    // should this be - request.sourceStartY?
    response->endingY = cy + maxy0 + request.sourceStartY; // definitely should be one lineheight higher
    
    return response;
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
