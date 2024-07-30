#include <DrzEngine_PGE.h>

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

void DrzEngine_PGE::Setup() {
  std::cout << "DrzEngine_PGE::Setup called" << std::endl;
  if(!pge->Construct(width, height, pixelSize, pixelSize)) {
    std::cerr << "Failed to construct PGE" << std::endl;
  }
}

void DrzEngine_PGE::Start() {
  std::cout << "DrzEngine_PGE::Start called" << std::endl;
  //start pge loop
  pge->Start();
  //TODO call started callback
}

#pragma endregion // IDrzEngine

#pragma region IDrzGraphics

void DrzEngine_PGE::DrawPixel(int x, int y, Color color) {
  //using pge graphics
  pge->Draw(x, y, olc::Pixel(color.r, color.g, color.b, color.a));
}

#pragma endregion // IDrzGraphics


