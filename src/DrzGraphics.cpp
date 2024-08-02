#include <DrzGraphics.h>
#include <cstdint>

namespace drz {

  using namespace graphics;

  #pragma region DrzGraphics

  IDrzGraphics* DrzGraphics::Get() {
    return instance;
  } 

  void DrzGraphics::Set(IDrzGraphics* graphics) {
    instance = graphics;
  }

  font* DrzGraphics::GetFont() {
    return currentFont;
  }

  font* DrzGraphics::GetFont(std::string fontName) {
    //look for font in map
    if(fonts.find(fontName) != fonts.end()) {
      return fonts[fontName];
    }
    return nullptr;
  }

  void DrzGraphics::SetFont(font* f) {
    currentFont = f;
  }

  void DrzGraphics::SetFont(std::string fontName) {
    //find font in map
    if(fonts.find(fontName) != fonts.end()) {
      currentFont = fonts[fontName];
    }
  }

  void DrzGraphics::LoadFont(std::string fontName, font* f) {
    //add font to map
    fonts[fontName] = f;
  }  

  rect DrzGraphics::GetTextBounds(const std::string& text, int x, int y) {
    if(!currentFont) {
      return {0, 0, 0, 0};
    }
    //call internal method
    int16_t x1, y1;
    uint16_t w1, h1;
    _getTextBounds(text, x, y, &x1, &y1, &w1, &h1);
    return {x1, y1, w1, h1};
  }

  void DrzGraphics::DrawText(const std::string& text, int x, int y, Color color) {
    if(!currentFont) {
      return;
    }
    //call internal method
    _writeText(text);
  }

  void DrzGraphics::_drawChar(uint16_t x, uint16_t y, unsigned char c, Color fg, Color bg) {
    c -= (unsigned char) currentFont->first;
    fontglyph *glyph = currentFont->glyph + c;
    uint8_t *bitmap = currentFont->bitmap;
    uint16_t bo = glyph->bitmapOffset;
    uint8_t w = glyph->width;
    uint8_t h = glyph->height;
    int8_t xo = glyph->xOffset;
    int8_t yo = glyph->yOffset;
    uint8_t xx, yy, bits = 0, bit = 0;
    int16_t xo16 = 0, yo16 = 0;
    for (yy = 0; yy < h; yy++) {
      for (xx = 0; xx < w; xx++) {
        if (!(bit++ & 7)) {
          bits = bitmap[bo++];
        }
        if (bits & 0x80) {
          instance->DrawPixel(x + xo + xx, y + yo + yy, fg);
        }
        bits <<= 1;
      }
    }
  }

  size_t DrzGraphics::_writeChar(unsigned char c) {
    if (c == '\n') {
      cursorX = 0;
      cursorY += currentFont->yAdvance;
    } else if (c != '\r') {
      uint8_t first = currentFont->first;
      if ((c >= first) && (c <= currentFont->last)) {
        fontglyph *glyph = (fontglyph*) (currentFont->glyph + c - first);
        uint8_t w = glyph->width;
        uint8_t h = glyph->height;
        if ((w > 0) && (h > 0)) { // Is there an associated bitmap?
          int16_t xo = glyph->xOffset; // sic
          if (textWrap && ((cursorX + xo + w) > instance->GetScreenWidth())) {
            cursorX = 0;
            cursorY += currentFont->yAdvance;
          }
          _drawChar(cursorX, cursorY, c, textFgColor, textBgColor);
        }
        cursorX += glyph->xAdvance;
      }
    }
    return 1;
  }

  size_t DrzGraphics::_writeTextBuffer(const char *buffer, size_t size) {
    size_t n = 0;
    while (size--) {
      if (_writeChar(*buffer++)) n++;
      else break;
    }
    return n;
  }

  size_t DrzGraphics::_writeText(const std::string &s) {
    return _writeTextBuffer(s.c_str(), s.length());
  }

  void DrzGraphics::_charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy) {
    if (!currentFont) {
      return;
    }
    // Newline?
    if (c == '\n') { 
      // Reset x to zero, advance y by one line
      *x = 0;       
      *y += currentFont->yAdvance;
    } 
    else if (c != '\r') { 
      // Not a carriage return; is normal char
      uint8_t first = currentFont->first;
      uint8_t last = currentFont->last;

      // Char present in this font?
      if ((c >= first) && (c <= last)) { 
        fontglyph *glyph = (fontglyph*) (currentFont->glyph +  (c - first));
        uint8_t gw = glyph->width;
        uint8_t gh = glyph->height;
        uint8_t xa = glyph->xAdvance;
        int8_t xo = glyph->xOffset;
        int8_t yo = glyph->yOffset;
        
        if (textWrap && ((*x + (((int16_t)xo + gw) * 1)) > instance->GetScreenWidth())) {
          // Reset x to zero, advance y by one line
          *x = 0; 
          *y += currentFont->yAdvance;
        }
        
        int16_t x1 = *x + xo * 1;
        int16_t y1 = *y + yo * 1;
        int16_t x2 = x1 + gw * 1 - 1;
        int16_t y2 = y1 + gh * 1 - 1;
        if (x1 < *minx)
          *minx = x1;
        if (y1 < *miny)
          *miny = y1;
        if (x2 > *maxx)
          *maxx = x2;
        if (y2 > *maxy)
          *maxy = y2;
        *x += xa * 1;
      }
    }
  }
  
  void DrzGraphics::_getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {

    uint8_t c; // Current character
    int16_t minx = 0x7FFF, miny = 0x7FFF, maxx = -1, maxy = -1; // Bound rect
    // Bound rect is intentionally initialized inverted, so 1st char sets it

    *x1 = x; // Initial position is value passed in
    *y1 = y;
    *w = *h = 0; // Initial size is zero

    while ((c = *str++)) {
      // charBounds() modifies x/y to advance for each character,
      // and min/max x/y are updated to incrementally build bounding rect.
      _charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);
    }

    if (maxx >= minx) {     // If legit string bounds were found...
      *x1 = minx;           // Update x1 to least X coord,
      *w = maxx - minx + 1; // And w to bound rect width
    }
    if (maxy >= miny) { // Same for height
      *y1 = miny;
      *h = maxy - miny + 1;
    }
  }

  void DrzGraphics::_getTextBounds(const std::string &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    if (str.length() != 0) {
      _getTextBounds(const_cast<char *>(str.c_str()), x, y, x1, y1, w, h);
    }
  }

  #pragma endregion DrzGraphics

  #pragma region Sprites

    Sprite::Sprite() {  
      width = 0; 
      height = 0; 
    }

    Sprite::Sprite(int32_t w, int32_t h) {		
      width = w;		height = h;
      colData.resize(width * height);
      colData.resize(width * height, defaultColor);
    }

    Sprite::Sprite(int32_t w, int32_t h, const unsigned int* data) {
      width = w;		height = h;
      colData.resize(width * height, defaultColor);
      int index = 0;
      for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
          int pixel = data[index++];
          Color p(
            (pixel >> 24) & 0xFF,   // R
            (pixel >> 16) & 0xFF, // G
            (pixel >>  8) & 0xFF,  // B
            (pixel >>  0) & 0xFF  // A
          );
          SetPixel(x, y, p);
        }
      }
    }

    Sprite::~Sprite() { 
      colData.clear();
    }
  #pragma endregion Sprites

} // namespace drz