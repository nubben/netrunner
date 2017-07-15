#ifndef TEXTRASTERIZERCACHE_H
#define TEXTRASTERIZERCACHE_H

#include "TextRasterizer.h"
#include <map>

class TextRasterizerCache {
private:
    std::map<unsigned int, std::shared_ptr<TextRasterizer> > fontSizes_bold;
    std::map<unsigned int, std::shared_ptr<TextRasterizer> > fontSizes_notbold;
public:
    std::shared_ptr<TextRasterizer> loadFont(const int size, const bool bold);
};

#endif