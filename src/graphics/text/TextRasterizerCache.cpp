#include "TextRasterizerCache.h"

TextRasterizerCache *rasterizerCache=new TextRasterizerCache;

// reduces this:
// Updated DOM in: 6.787870 seconds
// to this:
// Updated DOM in: 6.104453 seconds
std::shared_ptr<TextRasterizer> TextRasterizerCache::loadFont(const int size, const bool bold) {
    if (bold) {
        if (fontSizes_bold.find(size) == fontSizes_bold.end()) {
            fontSizes_bold[size]=std::make_shared<TextRasterizer>("DejaVuSerif.ttf", size, 72, bold);
        }
        return fontSizes_bold[size];
    } else {
        if (fontSizes_notbold.find(size) == fontSizes_notbold.end()) {
            fontSizes_notbold[size]=std::make_shared<TextRasterizer>("DejaVuSerif.ttf", size, 72, bold);
        }
        return fontSizes_notbold[size];
    }
}