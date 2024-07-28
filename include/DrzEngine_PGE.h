#pragma once

#include "DrzEngine.h"

#include <olcPixelGameEngine.h>

namespace drz {

class PixelGameEngineApp : public olc::PixelGameEngine
{
public:
    bool OnUserCreate() override
    {
      // Called once at the start, so create things here
      std::cout << "PGEapp OnUserCreate called" << std::endl;
      //TODO: call DrzEngine::setup here
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
      // Called once per frame
      //std::cout << "PGEapp OnUserUpdate called" << std::endl;
      //call loop callback
      loopCallBack(fElapsedTime);
      return true;
    }

    std::function<bool(float)> loopCallBack = [](float elapsedTime) {
      return true;
    };
};

class DrzEngine_PGE : public IDrzGraphics, public IDrzInputs, public IDrzEngine
{
  public:
    DrzEngine_PGE(int width, int height, int pixelSize);
    ~DrzEngine_PGE();

    void Setup() override;
    void Loop(float elapsedTime) override;
    void Start() override;

  private:
    PixelGameEngineApp* pge;
    int width;
    int height;
    int pixelSize;
};

} // namespace drz