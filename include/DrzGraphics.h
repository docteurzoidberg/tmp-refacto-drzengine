#pragma once

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <map>
#include <sys/types.h>

namespace drz::graphics {

  #pragma region Constants

  constexpr uint8_t  defaultAlpha = 0xFF;
  constexpr uint32_t defaultColor = (defaultAlpha << 24);
  
  #pragma endregion

  #pragma region Enums

  enum Mode { NORMAL, MASK, ALPHA, CUSTOM };

  #pragma endregion

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

  struct triangleref {
    vec3d* p[3];
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

    int32_t width = 0;
    int32_t height = 0;
    enum Mode { NORMAL, PERIODIC, CLAMP };
    enum Flip { NONE = 0, HORIZ = 1, VERT = 2 };
    std::vector<Color> colData;
    Mode modeSample = Mode::NORMAL;

    Sprite();
    Sprite(int32_t w, int32_t h);
    Sprite(int32_t w, int32_t h, const unsigned int* data);
    ~Sprite();

    void SetSampleMode(Mode mode) { 
      modeSample = mode; 
    }

    Color GetPixel(int32_t x, int32_t y) const
    {
      if (modeSample == Sprite::Mode::NORMAL)
      {
        if (x >= 0 && x < width && y >= 0 && y < height)
          return colData[y * width + x];
        else
          return Color(0, 0, 0, 0);
      }
      else
      {
        if (modeSample == Sprite::Mode::PERIODIC)
          return colData[abs(y % height) * width + abs(x % width)];
        else
          return colData[std::max(0, std::min(y, height-1)) * width + std::max(0, std::min(x, width-1))];
      }
    }

    bool SetPixel(int32_t x, int32_t y, Color p)
    {
      if (x >= 0 && x < width && y >= 0 && y < height)
      {
        colData[y * width + x] = p;
        return true;
      }
      else
        return false;
    }

    Color Sample(float x, float y) const
    {
      int32_t sx = std::min((int32_t)((x * (float)width)), width - 1);
      int32_t sy = std::min((int32_t)((y * (float)height)), height - 1);
      return GetPixel(sx, sy);
    }

    Color SampleBL(float u, float v) const
    {
      u = u * width - 0.5f;
      v = v * height - 0.5f;
      int x = (int)floor(u); // cast to int rounds toward zero, not downward
      int y = (int)floor(v); // Thanks @joshinils
      float u_ratio = u - x;
      float v_ratio = v - y;
      float u_opposite = 1 - u_ratio;
      float v_opposite = 1 - v_ratio;

      Color p1 = GetPixel(std::max(x, 0), std::max(y, 0));
      Color p2 = GetPixel(std::min(x + 1, (int)width - 1), std::max(y, 0));
      Color p3 = GetPixel(std::max(x, 0), std::min(y + 1, (int)height - 1));
      Color p4 = GetPixel(std::min(x + 1, (int)width - 1), std::min(y + 1, (int)height - 1));

      return Color(
        (uint8_t)((p1.r * u_opposite + p2.r * u_ratio) * v_opposite + (p3.r * u_opposite + p4.r * u_ratio) * v_ratio),
        (uint8_t)((p1.g * u_opposite + p2.g * u_ratio) * v_opposite + (p3.g * u_opposite + p4.g * u_ratio) * v_ratio),
        (uint8_t)((p1.b * u_opposite + p2.b * u_ratio) * v_opposite + (p3.b * u_opposite + p4.b * u_ratio) * v_ratio));
    }

    Color* GetData() {
      return colData.data();
    }
  };

  #pragma endregion Sprite

  #pragma region Model


  class Vector {
    public:
      static vec3d Add(vec3d &v1, vec3d &v2) {
        return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
      }

      static vec3d Sub(vec3d &v1, vec3d &v2) {
        return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
      }

      static vec3d Mul(vec3d &v1, float k) {
        return { v1.x * k, v1.y * k, v1.z * k };
      }

      static vec3d Div(vec3d &v1, float k) {
        return { v1.x / k, v1.y / k, v1.z / k };
      }

      static float DotProduct(vec3d &v1, vec3d &v2) {
        return v1.x*v2.x + v1.y*v2.y + v1.z * v2.z;
      }

      static float Length(vec3d &v) {
        return sqrtf(DotProduct(v, v));
      }

      static vec3d Normalise(vec3d &v) {
        float l = Length(v);
        return { v.x / l, v.y / l, v.z / l };
      }

      static vec3d CrossProduct(vec3d &v1, vec3d &v2) {
        vec3d v;
        v.x = v1.y * v2.z - v1.z * v2.y;
        v.y = v1.z * v2.x - v1.x * v2.z;
        v.z = v1.x * v2.y - v1.y * v2.x;
        return v;
      }

      static vec3d IntersectPlane(vec3d &plane_p, vec3d &plane_n, vec3d &lineStart, vec3d &lineEnd) {
        plane_n = Normalise(plane_n);
        float plane_d = -DotProduct(plane_n, plane_p);
        float ad = DotProduct(lineStart, plane_n);
        float bd =DotProduct(lineEnd, plane_n);
        float t = (-plane_d - ad) / (bd - ad);
        vec3d lineStartToEnd = Sub(lineEnd, lineStart);
        vec3d lineToIntersect = Mul(lineStartToEnd, t);
        return Add(lineStart, lineToIntersect);
      }
  };


  class Matrix4x4 {
    public:
      float m[4][4];

      static Matrix4x4 Identity() {
        Matrix4x4 matrix = {};
        for (int i = 0; i < 4; ++i) matrix.m[i][i] = 1.0f;
        return matrix;
      }

      static void MultiplyVector(Matrix4x4& m, const vec3d& i, vec3d& o) {
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];
        if (w != 0.0f) {
          o.x /= w; o.y /= w; o.z /= w;
        }
      }	

      static Matrix4x4 CreateRotationMatrixX(float angle) {
        Matrix4x4 matrix = Matrix4x4::Identity();
        matrix[1][1] = cos(angle);
        matrix[1][2] = -sin(angle);
        matrix[2][1] = sin(angle);
        matrix[2][2] = cos(angle);
        return matrix;
      }

      static Matrix4x4 CreateRotationMatrixY(float angle) {
        Matrix4x4 matrix = Matrix4x4::Identity();
        matrix[0][0] = cos(angle);
        matrix[0][2] = sin(angle);
        matrix[2][0] = -sin(angle);
        matrix[2][2] = cos(angle);
        return matrix;
      }

      static Matrix4x4 CreateRotationMatrixZ(float angle) {
        Matrix4x4 matrix = Matrix4x4::Identity();
        matrix[0][0] = cos(angle);
        matrix[0][1] = -sin(angle);
        matrix[1][0] = sin(angle);
        matrix[1][1] = cos(angle);
        return matrix;
      }

      static Matrix4x4 CreateRotationMatrix(float angleX, float angleY, float angleZ) {
        Matrix4x4 matrixX = Matrix4x4::CreateRotationMatrixX(angleX);
        Matrix4x4 matrixY = Matrix4x4::CreateRotationMatrixY(angleY);
        Matrix4x4 matrixZ = Matrix4x4::CreateRotationMatrixZ(angleZ);
        return matrixZ * matrixY * matrixX;
      }

      static Matrix4x4 CreateTranslationMatrix(float x, float y, float z) {
        Matrix4x4 matrix = Matrix4x4::Identity();
        matrix[0][3] = x;
        matrix[1][3] = y;
        matrix[2][3] = z;
        return matrix;
      }

      static Matrix4x4 MultiplyMatrix(Matrix4x4 &m1, Matrix4x4 &m2) {
        Matrix4x4 matrix;
        for (int c = 0; c < 4; c++)
          for (int r = 0; r < 4; r++)
            matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
        return matrix;
      }

      static Matrix4x4 PointAt(vec3d &pos, vec3d &target, vec3d &up) {
        // Calculate new forward direction
        vec3d newForward = Vector::Sub(target, pos);
        newForward = Vector::Normalise(newForward);

        // Calculate new Up direction
        vec3d a = Vector::Mul(newForward, Vector::DotProduct(up, newForward));
        vec3d newUp = Vector::Sub(up, a);
        newUp = Vector::Normalise(newUp);

        // New Right direction is easy, its just cross product
        vec3d newRight = Vector::CrossProduct(newUp, newForward);

        // Construct Dimensioning and Translation Matrix	
        Matrix4x4 matrix;
        matrix.m[0][0] = newRight.x;	matrix.m[0][1] = newRight.y;	matrix.m[0][2] = newRight.z;	matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = newUp.x;		matrix.m[1][1] = newUp.y;		matrix.m[1][2] = newUp.z;		matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = newForward.x;	matrix.m[2][1] = newForward.y;	matrix.m[2][2] = newForward.z;	matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
        return matrix;

      }

      // Only for Rotation/Translation Matrices
      static Matrix4x4 QuickInverse(Matrix4x4 &m) {
        Matrix4x4 matrix;
        matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
        matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
        matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
        matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
        matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
        matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
        matrix.m[3][3] = 1.0f;
        return matrix;
      }

      Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result = {};
        for (int i = 0; i < 4; ++i) {
          for (int j = 0; j < 4; ++j) {
            for (int k = 0; k < 4; ++k) {
              result.m[i][j] += m[i][k] * other.m[k][j];
            }
          }
        }
        return result;
      }

      float* operator[](int index) { return m[index]; }
      const float* operator[](int index) const { return m[index]; }
  };

  //Base class to load a 3d model
  class Model {
    public:
      Model(std::vector<vec3d>* verts, std::vector<face>* faces) : verts(verts), faces(faces){}

      virtual void Update(float fElapsedTime) = 0;

      void SetupModel() {
        //std::cout << "Vert count: " << verts->size() << std::endl;
        //std::cout << "Face count: " << faces->size() << std::endl;
        //std::cout << "Loading model" << std::endl;
        _loadTriRefs();
        //std::cout << "Model loaded with " << tris.size() << " triangles" << std::endl;
      }

      std::vector<vec3d>* verts;
      std::vector<face>* faces;
      std::vector<triangleref> tris;


      Matrix4x4 rotationMatrix;
      Matrix4x4 translationMatrix;
        
    private:
      void _loadTriRefs() {
        for (int i=0; i<faces->size(); i++) {
          face* f = &faces->at(i);
          vec3d* p0 = &verts->at(f->f[0] - 1);
          vec3d* p1 = &verts->at(f->f[1] - 1);
          vec3d* p2 = &verts->at(f->f[2] - 1);
          tris.push_back({
            p0, p1, p2
          });
        } 
      }
  };


  #pragma endregion Model

} // namespace drz::graphics

namespace drz {

  using namespace graphics;

  #pragma region IDrzGraphics
  
  class IDrzGraphics {
    public: 

      virtual void Clear(Color color) = 0;

      virtual void LoadFont(const std::string& fontName, font* font) = 0;

      virtual void SetPaintMode(Mode mode) = 0;
      virtual void SetFont(font* font) = 0;
      virtual void SetFont(std::string fontName) = 0;
      
      virtual bool DrawPixel(int x, int y, Color color) = 0;
      virtual void DrawLine(int x1, int y1, int x2, int y2, Color color) = 0;
      virtual void DrawRect(int x, int y, int width, int height, Color color) = 0;
      virtual void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) = 0;
      virtual void DrawText(std::string text, int x, int y, Color color) = 0;

      virtual void DrawSprite(int x, int y, Sprite* sprite) = 0;
      virtual void DrawPartialSprite(int x, int y, Sprite* sprite, int ox, int oy, int w, int h) = 0;
      virtual void DrawPartialSprite(vi2d pos, Sprite* sprite, vi2d srcPos, vi2d size) = 0;

      virtual void FillRect(int x, int y, int width, int height,Color color) = 0;
      virtual void FillCircle(int x, int y, int radius, Color color) = 0;
      virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) = 0;
      
      virtual rect GetTextBounds(const std::string& text, int x, int y) = 0;

      virtual int GetScreenWidth() = 0;
      virtual int GetScreenHeight() = 0;
  };

  #pragma endregion IDrzGraphics
  
  #pragma region DrzGraphics

  class DrzGraphics {
    public:
      static IDrzGraphics* Get();
      static void Set(IDrzGraphics* graphics);

      static void LoadFont(const std::string& fontName, font* f);
      static font* GetFont();
      static font* GetFont(std::string fontName);
      static void SetFont(font* f);
      static void SetFont(std::string fontName);

      static void SetCursorPos(int x, int y);
      static void SetTextWrap(bool wrap);
      static void SetTextColor(Color fg, Color bg);
      static void SetTextForegroundColor(Color fg);
      static void SetTextBackgroundColor(Color bg);

      static void DrawText(const std::string& text, int x, int y, Color color);

      static rect GetTextBounds(const std::string& text, int x, int y);

    private:
      inline static int cursorX = 0;
      inline static int cursorY = 0;
      inline static bool textWrap = true;
      inline static Color textFgColor = WHITE;
      inline static Color textBgColor = BLACK;
      inline static IDrzGraphics* instance = nullptr;
      inline static std::map<std::string, font*> fonts;
      inline static font* currentFont = nullptr;

      inline static void _drawChar(uint16_t x, uint16_t y, unsigned char c, Color fg, Color bg);

      inline static size_t _writeChar(unsigned char c);
      inline static size_t _writeTextBuffer(const char *buffer, size_t size);
      inline static size_t _writeText(const std::string &s);

      inline static void _charBounds(unsigned char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
      inline static void _getTextBounds(const char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
      inline static void _getTextBounds(const std::string &str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
  };

  #pragma endregion DrzGraphics

} // namespace drz