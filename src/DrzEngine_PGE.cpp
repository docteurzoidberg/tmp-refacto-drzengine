#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>
#include <DrzInputs.h>
#include <DrzSerial.h>

#include <IDrzSerial.h>

#include <cstdint>

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

#ifdef _WIN32
#include <DrzSerial_Win.h>
#endif

#ifdef __EMSCRIPTEN__
#include <DrzSerial_Null.h>
#endif

#ifdef __linux__
#include <DrzSerial_Linux.h>
#endif

using namespace drz;

DrzEngine_PGE::DrzEngine_PGE(int width, int height, int pixelSize) : width(width), height(height), pixelSize(pixelSize) {

  std::cout << "DrzEngine_PGE constructor called" << std::endl;

  pge = new PixelGameEngineApp();

  //PGEengine does graphics, main loop, and inputs
  DrzEngine::Set(this);
  DrzGraphics::Set(this);
  DrzInputs::Set(this);

  //TODO: set serial class
  
  #ifdef _WIN32
    new DrzSerial_Win();
  #elif __linux__
  //TODO: include linux serial
    new DrzSerial_Linux();
  #elif __EMSCRIPTEN__
  //TODO: include null serial
  #endif

  //TODO: set audio class
}

#pragma region IDrzEngine

DrzEngine_PGE::~DrzEngine_PGE() {
  std::cout << "DrzEngine_PGE destructor called" << std::endl;
  //start chrono

}

float DrzEngine_PGE::GetRandomFloat() {
  //returns random float between 0 and 1 using std::rand
  return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
}

uint32_t DrzEngine_PGE::Now() {
  auto now = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - start);
  return duration.count();
}

void DrzEngine_PGE::Setup() {
  std::cout << "DrzEngine_PGE::Setup called" << std::endl;
  if(!pge->Construct(width, height, pixelSize, pixelSize)) {
    std::cerr << "Failed to construct PGE" << std::endl;
  }
  //pge->ConsoleCaptureStdOut(true);
}

void DrzEngine_PGE::Start() {
  std::cout << "DrzEngine_PGE::Start called" << std::endl;
  //start pge loop
  start = std::chrono::high_resolution_clock::now();
  pge->Start();
}

#pragma endregion

#pragma region IDrzInputs

HardwareButton DrzEngine_PGE::GetKey(Key key) {
  auto button = pge->GetKey(static_cast<olc::Key>((int)key));
  HardwareButton hwButton;
  hwButton.isPressed = button.bPressed;
  hwButton.isReleased = button.bReleased;
  hwButton.isHeld = button.bHeld;
  return hwButton;
}

#pragma endregion


#pragma region IDrzGraphics

void DrzEngine_PGE::LoadFont(const std::string& fontName, font* f) {
  DrzGraphics::LoadFont(fontName, f);
}

void DrzEngine_PGE::SetPaintMode(Mode mode) {
  if(mode == Mode::ALPHA)
    pge->SetPixelMode(olc::Pixel::ALPHA);
  else if(mode == Mode::MASK)
    pge->SetPixelMode(olc::Pixel::MASK);
  else if(mode == Mode::NORMAL)
    pge->SetPixelMode(olc::Pixel::NORMAL);
  else if(mode == Mode::CUSTOM)
    pge->SetPixelMode(olc::Pixel::CUSTOM);
  else
    pge->SetPixelMode(olc::Pixel::NORMAL);
}

void DrzEngine_PGE::SetFont(font* font) {
  DrzGraphics::SetFont(font);
}

void DrzEngine_PGE::SetFont(std::string fontName) {
  DrzGraphics::SetFont(fontName);
}

void DrzEngine_PGE::Clear(Color color) {
  pge->Clear(olc::Pixel(color.r, color.g, color.b, color.a));
}

bool DrzEngine_PGE::DrawPixel(int x, int y, Color color) {
  return pge->Draw(x, y, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawLine(int x1, int y1, int x2, int y2, Color color) {
  pge->DrawLine(x1, y1, x2, y2, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawRect(int x, int y, int width, int height, Color color) {
  pge->DrawRect(x, y, width, height, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  pge->DrawTriangle(x1, y1, x2, y2, x3, y3, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::DrawText(std::string text, int x, int y, Color color) {
  //pge->DrawString(x, y, text, olc::Pixel(color.r, color.g, color.b, color.a));
    auto currentFont = DrzGraphics::GetFont();
    if(currentFont==nullptr) {
      std::cout << "[!] No font set" << std::endl;
      return;
    } 
    DrzGraphics::SetCursorPos((int) x, (int) y);
    DrzGraphics::SetTextForegroundColor(color);
    DrzGraphics::DrawText(text, x, y, color);
}

void DrzEngine_PGE::DrawSprite(int32_t x, int32_t y, drz::graphics::Sprite* sprite) {
  pge->DrawSprite(x, y, (olc::Sprite*)sprite, 1);
}

void DrzEngine_PGE::DrawPartialSprite(int32_t x, int32_t y, drz::graphics::Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h) {
  pge->DrawPartialSprite(x, y, (olc::Sprite*)sprite,ox, oy, w, h, 1, 0);
}

void DrzEngine_PGE::DrawPartialSprite(vi2d pos, drz::graphics::Sprite* sprite, vi2d srcPos, vi2d size) {
  DrzEngine_PGE::DrawPartialSprite(pos.x, pos.y, sprite, srcPos.x, srcPos.y, size.x, size.y);
}

void DrzEngine_PGE::FillRect(int x, int y, int width, int height, Color color) {
  pge->FillRect(x, y, width, height, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::FillCircle(int x, int y, int radius, Color color) {
  pge->FillCircle(x, y, radius, olc::Pixel(color.r, color.g, color.b, color.a));
}

void DrzEngine_PGE::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  pge->FillTriangle(x1, y1, x2, y2, x3, y3, olc::Pixel(color.r, color.g, color.b, color.a));
}

rect DrzEngine_PGE::GetTextBounds(const std::string& text, int x, int y) {
  return DrzGraphics::GetTextBounds(text, x, y);
}

#pragma endregion



