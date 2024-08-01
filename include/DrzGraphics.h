#pragma once

#include <cstdint>
#include <algorithm>
#include <map>

namespace drz::graphics {

  #pragma region Constants
  constexpr uint8_t  defaultAlpha = 0xFF;
  constexpr uint32_t defaultColor = (defaultAlpha << 24);
  #pragma endregion

  #pragma region Enums
  enum Mode { NORMAL, MASK, ALPHA, CUSTOM };
  #pragma endregion

  //Color class-like struct (from olc::pge)
  #pragma region Color

  /// @brief Color data structure used in all graphics methods
  struct Color {
    union {
      uint32_t n = defaultColor;
      struct { uint8_t r; uint8_t g; uint8_t b; uint8_t a; };
    };

    Color();
    Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = defaultAlpha);
    Color(uint32_t p);
    Color& operator = (const Color& v) = default;
    bool   operator ==(const Color& p) const;
    bool   operator !=(const Color& p) const;
    Color  operator * (const float i) const;
    Color  operator / (const float i) const;
    Color& operator *=(const float i);
    Color& operator /=(const float i);
    Color  operator + (const Color& p) const;
    Color  operator - (const Color& p) const;
    Color& operator +=(const Color& p);
    Color& operator -=(const Color& p);
    Color  operator * (const Color& p) const;
    Color& operator *=(const Color& p);
    Color  inv() const;

    uint8_t toRGB332() const { 
      return ((r & 0xE0) >> 5) | ((g & 0xE0) >> 2) | (b & 0xC0); 
    }
  };

  Color ColorF(float red, float green, float blue, float alpha = 1.0f);
  Color ColorLerp(const Color& p1, const Color& p2, float t);

  /// @brief Predefined colors
  static const Color
    GREY(192, 192, 192), 
    DARK_GREY(128, 128, 128), 
    VERY_DARK_GREY(64, 64, 64),
    RED(255, 0, 0), 
    DARK_RED(128, 0, 0), 
    VERY_DARK_RED(64, 0, 0),
    YELLOW(255, 255, 0), 
    DARK_YELLOW(128, 128, 0), 
    VERY_DARK_YELLOW(64, 64, 0),
    GREEN(0, 255, 0), 
    DARK_GREEN(0, 128, 0), 
    VERY_DARK_GREEN(0, 64, 0),
    CYAN(0, 255, 255), 
    DARK_CYAN(0, 128, 128), 
    VERY_DARK_CYAN(0, 64, 64),
    BLUE(0, 0, 255), 
    DARK_BLUE(0, 0, 128), 
    VERY_DARK_BLUE(0, 0, 64),
    MAGENTA(255, 0, 255), 
    DARK_MAGENTA(128, 0, 128), 
    VERY_DARK_MAGENTA(64, 0, 64),
    WHITE(255, 255, 255), 
    BLACK(0, 0, 0), 
    BLANK(0, 0, 0, 0)
  ;

  inline Color::Color() { 
    r = 0; g = 0; b = 0; a = defaultAlpha; 
  }

  inline Color::Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) { 
    n = red | (green << 8) | (blue << 16) | (alpha << 24);  // Thanks jarekpelczar 
  }

  inline Color::Color(uint32_t p) { 
    n = p; 
  }

  inline bool Color::operator==(const Color& p) const {
    return n == p.n; 
  }

  inline bool Color::operator!=(const Color& p) const {
    return n != p.n; 
  }

  inline Color Color::operator * (const float i) const {
    float fR = std::min(255.0f, std::max(0.0f, float(r) * i));
    float fG = std::min(255.0f, std::max(0.0f, float(g) * i));
    float fB = std::min(255.0f, std::max(0.0f, float(b) * i));
    return Color(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
  }

  inline Color Color::operator / (const float i) const {
    float fR = std::min(255.0f, std::max(0.0f, float(r) / i));
    float fG = std::min(255.0f, std::max(0.0f, float(g) / i));
    float fB = std::min(255.0f, std::max(0.0f, float(b) / i));
    return Color(uint8_t(fR), uint8_t(fG), uint8_t(fB), a);
  }

  inline Color& Color::operator *=(const float i) {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * i)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * i)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * i)));
    return *this;
  }

  inline Color& Color::operator /=(const float i) {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) / i)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) / i)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) / i)));
    return *this;
  }

  inline Color Color::operator + (const Color& p) const {
    uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
    return Color(nR, nG, nB, a);
  }

  inline Color Color::operator - (const Color& p) const {
    uint8_t nR = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
    return Color(nR, nG, nB, a);
  }

  inline Color& Color::operator += (const Color& p) {
    this->r = uint8_t(std::min(255, std::max(0, int(r) + int(p.r))));
    this->g = uint8_t(std::min(255, std::max(0, int(g) + int(p.g))));
    this->b = uint8_t(std::min(255, std::max(0, int(b) + int(p.b))));
    return *this;
  }

  inline Color& Color::operator -= (const Color& p) {
    this->r = uint8_t(std::min(255, std::max(0, int(r) - int(p.r))));
    this->g = uint8_t(std::min(255, std::max(0, int(g) - int(p.g))));
    this->b = uint8_t(std::min(255, std::max(0, int(b) - int(p.b))));
    return *this;
  }

  inline Color Color::operator * (const Color& p) const {
    uint8_t nR = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * float(p.r) / 255.0f)));
    uint8_t nG = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * float(p.g) / 255.0f)));
    uint8_t nB = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * float(p.b) / 255.0f)));
    uint8_t nA = uint8_t(std::min(255.0f, std::max(0.0f, float(a) * float(p.a) / 255.0f)));
    return Color(nR, nG, nB, nA);
  }

  inline Color& Color::operator *=(const Color& p) {
    this->r = uint8_t(std::min(255.0f, std::max(0.0f, float(r) * float(p.r) / 255.0f)));
    this->g = uint8_t(std::min(255.0f, std::max(0.0f, float(g) * float(p.g) / 255.0f)));
    this->b = uint8_t(std::min(255.0f, std::max(0.0f, float(b) * float(p.b) / 255.0f)));
    this->a = uint8_t(std::min(255.0f, std::max(0.0f, float(a) * float(p.a) / 255.0f)));
    return *this;
  }

  inline Color Color::inv() const {
    uint8_t nR = uint8_t(std::min(255, std::max(0, 255 - int(r))));
    uint8_t nG = uint8_t(std::min(255, std::max(0, 255 - int(g))));
    uint8_t nB = uint8_t(std::min(255, std::max(0, 255 - int(b))));
    return Color(nR, nG, nB, a);
  }

  inline Color ColorF(float red, float green, float blue, float alpha) {
    return Color(uint8_t(red * 255.0f), uint8_t(green * 255.0f), uint8_t(blue * 255.0f), uint8_t(alpha * 255.0f)); 
  }

  inline Color ColorLerp(const Color& p1, const Color& p2, float t) {
    return (p2 * t) + p1 * (1.0f - t);
  }

  #pragma endregion // Color

  #pragma region Structs

  struct rect {
    int x, y, w, h;
  };

  struct vec2d {
    float x, y;
  };

  struct vec3d {
    float x, y, z, w;
  };

  struct vi2d {
    int x, y;
  };

  struct face {
    int f[3];
  };

  struct triangle {
    vec3d p[3];
    Color color;
  };

  #pragma endregion Structs

  #pragma region Fonts

  /// Font glyph structure (taken from Adafruit GFX library)
  struct fontglyph {
    uint16_t bitmapOffset;  // Pointer into GFXfont->bitmap
    uint8_t width;
    uint8_t height;         // Bitmap dimensions in pixels
    uint8_t xAdvance;       // Distance to advance cursor (x axis)
    int8_t xOffset;
    int8_t yOffset;         // Dist from cursor pos to UL corner
  };

  /// Font structure (take from Adafruit GFX library)
  struct font {
    uint8_t *bitmap;        // Glyph bitmaps, concatenated
    fontglyph *glyph;       // Glyph array
    uint16_t first;
    uint16_t last;          // ASCII extents
    uint8_t yAdvance;       // Newline distance (y axis)
  };

  #pragma endregion Fonts

  #pragma region Sprite

  class Sprite {
    public:
      Sprite();
      Sprite(int w, int h);
      Sprite(int w, int h, const uint32_t* data);
  };

  #pragma endregion Sprite

} // namespace drz::graphics

namespace drz {

  using namespace graphics;

  #pragma region IDrzGraphics
  
  class IDrzGraphics {
    public: 

      virtual void Clear(Color color) = 0;

      virtual void LoadFont(std::string fontName, font* f) = 0;
      virtual void SetFont(std::string fontName) = 0;
      
      virtual bool DrawPixel(int x, int y, Color color) = 0;
      virtual void DrawLine(int x1, int y1, int x2, int y2, Color color) = 0;
      virtual void DrawRect(int x, int y, int width, int height, Color color) = 0;
      virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) = 0;
      virtual void DrawText(std::string text, int x, int y, Color color) = 0;
      
      virtual void DrawPartialSprite(int x, int y, Sprite* sprite, int ox, int oy, int w, int h) = 0;
      virtual void DrawPartialSprite(vi2d pos, Sprite* sprite, vi2d srcPos, vi2d size) = 0;

      virtual void FillRect(int x, int y, int width, int height,Color color) = 0;
      virtual void FillCircle(int x, int y, int radius, Color color) = 0;
      virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) = 0;
      
      virtual int GetScreenWidth() = 0;
      virtual int GetScreenHeight() = 0;
  };

  #pragma endregion IDrzGraphics
  
  #pragma region DrzGraphics

  class DrzGraphics {
    public:
      static IDrzGraphics* Get();
      static void Set(IDrzGraphics* graphics);

      static void LoadFont(std::string fontName, font* f);
      static font* GetFont();
      static font* GetFont(std::string fontName);
      static void SetFont(font* f);
      static void SetFont(std::string fontName);

    private:
      inline static IDrzGraphics* instance = nullptr;
      inline static std::map<std::string, font*> fonts;
      inline static font* currentFont = nullptr;
  };

  #pragma endregion DrzGraphics

} // namespace drz