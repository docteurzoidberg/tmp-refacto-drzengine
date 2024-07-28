#include <DrzEngine_PGE.h>

#define OLC_PGE_APPLICATION
#include <olcPixelGameEngine.h>

using namespace drz;

DrzEngine_PGE::DrzEngine_PGE(int width, int height, int pixelSize) : width(width), height(height), pixelSize(pixelSize) {

  std::cout << "DrzEngine_PGE constructor called" << std::endl;

  pge = new PixelGameEngineApp();

  //Set pge loop callback
  pge->loopCallBack = [&](float fElapsedTime) {
    Loop(fElapsedTime);
    return true;
  };
  
  DrzEngine::Set(this);
  DrzGraphics::Set(this);
  DrzInputs::Set(this);

  //TODO: set serial class
  //TODO: set audio class
}

DrzEngine_PGE::~DrzEngine_PGE() {
  std::cout << "DrzEngine_PGE destructor called" << std::endl;
}

void DrzEngine_PGE::Setup() {
  std::cout << "DrzEngine_PGE::Setup called" << std::endl;
  if(!pge->Construct(width, height, pixelSize, pixelSize)) {
    std::cerr << "Failed to construct PGE" << std::endl;
  }
}

void DrzEngine_PGE::Loop(float elapsedTime) {
  //std::cout << "DrzEngine_PGE::Loop called" << std::endl;
  //Trigger engine loop
  DrzEngine::Loop(elapsedTime);
}

void DrzEngine_PGE::Start() {
  std::cout << "DrzEngine_PGE::Start called" << std::endl;
  pge->Start();
  //TODO call started callback
}


