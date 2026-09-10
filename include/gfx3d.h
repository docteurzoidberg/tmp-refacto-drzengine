#pragma once

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

// #include "olcPixelGameEngine.h"

#include <DrzGraphics.h>

#undef min
#undef max

// #include <omp.h>

namespace drz {
// Container class for Advanced 2D Drawing functions
class GFX3D {

public:
  struct vec2d {
    float x = 0;
    float y = 0;
    float z = 0;
  };

  struct vec3d {
    float x = 0;
    float y = 0;
    float z = 0;
    float w =
        1; // Need a 4th term to perform sensible matrix vector multiplication
  };

  struct triangle {
    drz::GFX3D::vec3d p[3];
    drz::GFX3D::vec2d t[3];
    drz::Color col[3];
  };

  struct mat4x4 {
    float m[4][4] = {0};
  };

  struct mesh {
    std::vector<drz::GFX3D::triangle> tris;
    // bool LoadOBJFile(std::string sFilename, bool bHasTexture = false);
  };

  /*class MipMap : public drz::Sprite
  {
  public:
          MipMap();
          MipMap(std::string sImageFile);
          MipMap(std::string sImageFile, drz::ResourcePack *pack);
          MipMap(int32_t w, int32_t h);
          ~MipMap();

  public:
          drz::rcode LoadFromFile(std::string sImageFile, drz::ResourcePack
  *pack = nullptr); drz::rcode LoadFromPGESprFile(std::string sImageFile,
  drz::ResourcePack *pack = nullptr); Pixel Sample(float x, float y, float z);
          Pixel SampleBL(float u, float v, float z);

  private:
          int GenerateMipLevels();
          std::vector<drz::Sprite> vecMipMaps;

  };*/

  class Math {
  public:
    Math();

  public:
    static vec3d Mat_MultiplyVector(mat4x4 &m, vec3d &i);
    static mat4x4 Mat_MultiplyMatrix(mat4x4 &m1, mat4x4 &m2);
    static mat4x4 Mat_MakeIdentity();
    static mat4x4 Mat_MakeRotationX(float fAngleRad);
    static mat4x4 Mat_MakeRotationY(float fAngleRad);
    static mat4x4 Mat_MakeRotationZ(float fAngleRad);
    static mat4x4 Mat_MakeScale(float x, float y, float z);
    static mat4x4 Mat_MakeTranslation(float x, float y, float z);
    static mat4x4 Mat_MakeProjection(float fFovDegrees, float fAspectRatio,
                                     float fNear, float fFar);
    static mat4x4 Mat_PointAt(vec3d &pos, vec3d &target, vec3d &up);
    static mat4x4
    Mat_QuickInverse(mat4x4 &m); // Only for Rotation/Translation Matrices
    static mat4x4 Mat_Inverse(drz::GFX3D::mat4x4 &m);

    static vec3d Vec_Add(vec3d &v1, vec3d &v2);
    static vec3d Vec_Sub(vec3d &v1, vec3d &v2);
    static vec3d Vec_Mul(vec3d &v1, float k);
    static vec3d Vec_Div(vec3d &v1, float k);
    static float Vec_DotProduct(vec3d &v1, vec3d &v2);
    static float Vec_Length(vec3d &v);
    static vec3d Vec_Normalise(vec3d &v);
    static vec3d Vec_CrossProduct(vec3d &v1, vec3d &v2);
    static vec3d Vec_IntersectPlane(vec3d &plane_p, vec3d &plane_n,
                                    vec3d &lineStart, vec3d &lineEnd, float &t);

    static int Triangle_ClipAgainstPlane(vec3d plane_p, vec3d plane_n,
                                         triangle &in_tri, triangle &out_tri1,
                                         triangle &out_tri2);
  };

  enum RENDERFLAGS {
    RENDER_WIRE = 0x01,
    RENDER_FLAT = 0x02,
    RENDER_TEXTURED = 0x04,
    RENDER_CULL_CW = 0x08,
    RENDER_CULL_CCW = 0x10,
    RENDER_DEPTH = 0x20,
    RENDER_LIGHTS = 0x40,
  };

  enum LIGHTS { LIGHT_DISABLED, LIGHT_AMBIENT, LIGHT_DIRECTIONAL, LIGHT_POINT };

  class Model {
  public:
    Model(std::vector<drz::vec3d> *verts, std::vector<drz::face> *faces,
          std::string name)
        : verts(verts), faces(faces), name(name) {}

    virtual void Update(float fElapsedTime) = 0;

    void SetupModel() {
      _loadTris();
      rotationMatrix = GFX3D::Math::Mat_MakeIdentity();
      translationMatrix = GFX3D::Math::Mat_MakeIdentity();
    }

    std::vector<drz::vec3d> *verts;
    std::vector<drz::face> *faces;
    std::vector<GFX3D::triangle> tris;

    drz::Color col = drz::WHITE;

    // Matrix4x4 rotationMatrix;
    // Matrix4x4 translationMatrix;
    GFX3D::mat4x4 rotationMatrix;
    GFX3D::mat4x4 translationMatrix;

    std::string name = "Model";

  private:
    void _loadTris() {

#ifdef EXPORT_TRIANGLES
      std::cout << "Vert count: " << verts->size() << std::endl;
      std::cout << "Face count: " << faces->size() << std::endl;
      std::cout << "Loading model " << name << std::endl;

      // outputing header definition for tris
      std::cout << "std::vector<drz::GFX3D::triangle> tris = {" << std::endl;
    }
#endif

    for (int i = 0; i < faces->size(); i++) {
      face *f = &faces->at(i);
      drz::vec3d *p0 = &verts->at(f->f[0] - 1);
      drz::vec3d *p1 = &verts->at(f->f[1] - 1);
      drz::vec3d *p2 = &verts->at(f->f[2] - 1);
      GFX3D::triangle tri;
      GFX3D::vec3d pn0 = {p0->x, p0->y, p0->z, 1.0f};
      GFX3D::vec3d pn1 = {p1->x, p1->y, p1->z, 1.0f};
      GFX3D::vec3d pn2 = {p2->x, p2->y, p2->z, 1.0f};
      tri.p[0] = pn0;
      tri.p[1] = pn1;
      tri.p[2] = pn2;
      tri.t[0] = {0, 0};
      tri.t[1] = {0, 0};
      tri.t[2] = {0, 0};
      tri.col[0] = drz::WHITE;
      tri.col[1] = drz::WHITE;
      tri.col[2] = drz::WHITE;
      tris.push_back(tri);

// export GFX3D::triangle initialization code
#ifdef EXPORT_TRIANGLES
      std::cout << "  {";
      std::cout << "    {" << p0->x << ", " << p0->y << ", " << p0->z << "}, "
                << std::endl;
      std::cout << "    {" << p1->x << ", " << p1->y << ", " << p1->z << "}, "
                << std::endl;
      std::cout << "    {" << p2->x << ", " << p2->y << ", " << p2->z << "}, "
                << std::endl;
      std::cout << "    {" << tri.t[0].x << ", " << tri.t[0].y << "}, "
                << std::endl;
      std::cout << "    {" << tri.t[1].x << ", " << tri.t[1].y << "}, "
                << std::endl;
      std::cout << "    {" << tri.t[2].x << ", " << tri.t[2].y << "}, "
                << std::endl;
      std::cout << "  {drz::WHITE,drz::WHITE, drz::WHITE}}," << std::endl;
#endif
    }

#ifdef EXPORT_TRIANGLES
    std::cout << "};" << std::endl;
    std::cout << "Model has " << tris.size() << " triangles." << std::endl;
#endif
  }
};

class PipeLine {
public:
  PipeLine();

public:
  void SetProjection(float fFovDegrees, float fAspectRatio, float fNear,
                     float fFar, float fLeft, float fTop, float fWidth,
                     float fHeight);
  void SetCamera(drz::GFX3D::vec3d &pos, drz::GFX3D::vec3d &lookat,
                 drz::GFX3D::vec3d &up);
  void SetTransform(drz::GFX3D::mat4x4 &transform);
  void SetTexture(drz::graphics::Sprite *texture);
  // void SetMipMapTexture(drz::GFX3D::MipMap *texture);
  void SetLightSource(uint32_t nSlot, uint32_t nType, drz::Color col,
                      drz::GFX3D::vec3d pos,
                      drz::GFX3D::vec3d dir = {0.0f, 0.0f, 1.0f, 1.0f},
                      float fParam = 0.0f);
  uint32_t Render(std::vector<drz::GFX3D::triangle> &triangles,
                  uint32_t flags = RENDER_CULL_CW | RENDER_TEXTURED |
                                   RENDER_DEPTH);
  uint32_t Render(std::vector<drz::GFX3D::triangle> &triangles, uint32_t flags,
                  int nOffset, int nCount);
  uint32_t RenderLine(drz::GFX3D::vec3d &p1, drz::GFX3D::vec3d &p2,
                      drz::Color col = drz::WHITE);
  uint32_t RenderCircleXZ(drz::GFX3D::vec3d &p1, float r,
                          drz::Color col = drz::WHITE);

private:
  IDrzGraphics *gfx =
      nullptr; // Pointer to the graphics engine, set by the main application

  drz::GFX3D::mat4x4 matProj;
  drz::GFX3D::mat4x4 matView;
  drz::GFX3D::mat4x4 matWorld;
  drz::Sprite *sprTexture;
  // drz::GFX3D::MipMap *sprMipMap;
  // bool bUseMipMap;
  float fViewX;
  float fViewY;
  float fViewW;
  float fViewH;

  // Value-initialised: the constructor never touched this array, so every
  // slot the caller did not set held an indeterminate `type`. Render()
  // switches on it, so a garbage slot could silently register itself as an
  // ambient or directional light with a garbage colour and direction and
  // corrupt the shading of arbitrary faces. 0 == LIGHT_DISABLED.
  struct sLight {
    uint32_t type;
    drz::GFX3D::vec3d pos;
    drz::GFX3D::vec3d dir;
    drz::Color col;
    float param;
  } lights[4] = {};
};

public:
// static const int RF_TEXTURE = 0x00000001;
// static const int RF_ = 0x00000002;

static void ConfigureDisplay(IDrzGraphics *gfx);
static void ClearDepth();
static void AddTriangleToScene(drz::GFX3D::triangle &tri);
static void RenderScene();

static void DrawTriangleFlat(drz::GFX3D::triangle &tri);
static void DrawTriangleWire(drz::GFX3D::triangle &tri,
                             drz::Color col = drz::WHITE);
static void DrawTriangleTex(drz::GFX3D::triangle &tri, drz::Sprite *spr);
static void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
                             int x2, int y2, float u2, float v2, float w2,
                             int x3, int y3, float u3, float v3, float w3,
                             drz::Sprite *spr);

static void RasterTriangle(int x1, int y1, float u1, float v1, float w1,
                           drz::Color c1, int x2, int y2, float u2, float v2,
                           float w2, drz::Color c2, int x3, int y3, float u3,
                           float v3, float w3, drz::Color c3, drz::Sprite *spr,
                           uint32_t nFlags);

// Draws a sprite with the transform applied
// inline static void DrawSprite(drz::Sprite *sprite, drz::GFX2D::Transform2D
// &transform);

private:
static float *m_DepthBuffer;
static IDrzGraphics *_gfx;
// Screen size cached by ConfigureDisplay: the rasteriser indexes the depth
// buffer per pixel, and GetScreenWidth() is a virtual call.
static int _screenW;
static int _screenH;
};
} // namespace drz
