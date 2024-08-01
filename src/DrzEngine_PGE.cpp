#include <DrzEngine_PGE.h>
#include <cstdint>

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

using namespace drz;

DrzEngine_PGE::DrzEngine_PGE(int width, int height, int pixelSize) : width(width), height(height), pixelSize(pixelSize) {

  std::cout << "DrzEngine_PGE constructor called" << std::endl;

  pge = new PixelGameEngineApp();

  //PGEengine does graphics, main loop, and inputs
  DrzEngine::Set(this);
  DrzGraphics::Set(this);
  DrzInputs::Set(this);

  //TODO: set serial class
  //TODO: set audio class
}


#pragma region IDrzEngine

DrzEngine_PGE::~DrzEngine_PGE() {
  std::cout << "DrzEngine_PGE destructor called" << std::endl;
}

float DrzEngine_PGE::GetRandomFloat() {
  //returns random float between 0 and 1 using std::rand
  return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

void DrzEngine_PGE::Setup() {
  std::cout << "DrzEngine_PGE::Setup called" << std::endl;
  if(!pge->Construct(width, height, pixelSize, pixelSize)) {
    std::cerr << "Failed to construct PGE" << std::endl;
  }
  pge->ConsoleCaptureStdOut(true);
}

void DrzEngine_PGE::Start() {
  std::cout << "DrzEngine_PGE::Start called" << std::endl;
  //start pge loop
  pge->Start();
  //TODO call started callback
}

#pragma endregion // IDrzEngine

#pragma region IDrzGraphics

bool DrzEngine_PGE::DrawPixel(int x, int y, Color color) {
  //using pge graphics
  return pge->Draw(x, y, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawLine(int x1, int y1, int x2, int y2, Color color) {
  //using pge graphics
  pge->DrawLine(x1, y1, x2, y2, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawRect(int x, int y, int width, int height, Color color) {
  //using pge graphics
  pge->DrawRect(x, y, width, height, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  //using pge graphics
  pge->DrawTriangle(x1, y1, x2, y2, x3, y3, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawText(std::string text, int x, int y, Color color) {
  //using pge graphics
  pge->DrawString(x, y, text, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawPartialSprite(int32_t x, int32_t y, drz::graphics::Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h) {
  //using pge graphics
  pge->DrawPartialSprite(x, y, (olc::Sprite*)sprite,ox, oy, w, h, 1, 0);
}

void DrzEngine_PGE::DrawPartialSprite(vi2d pos, drz::graphics::Sprite* sprite, vi2d srcPos, vi2d size) {
  //using pge graphics
  pge->DrawPartialSprite(pos.x, pos.y, (olc::Sprite*)sprite, srcPos.x, srcPos.y, size.x, size.y, 1, 0);
}

void DrzEngine_PGE::FillRect(int x, int y, int width, int height, Color color) {
  //using pge graphics
  pge->FillRect(x, y, width, height, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::FillCircle(int x, int y, int radius, Color color) {
  //using pge graphics
  pge->FillCircle(x, y, radius, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  //using pge graphics
  pge->FillTriangle(x1, y1, x2, y2, x3, y3, olc::Pixel(color.r, color.g, color.b, color.a));
}

#pragma endregion // IDrzGraphics


