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
      DrzEngine::Setup();
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
      // Called once per frame
      DrzEngine::Loop(fElapsedTime);
      return true;
    }
};

class DrzEngine_PGE : public IDrzGraphics, public IDrzInputs, public IDrzEngine
{
  public:
    DrzEngine_PGE(int width, int height, int pixelSize);

     //DrzEngine impl.
  #pragma region IDrzEngine
    ~DrzEngine_PGE();
    void Setup() override;
    void Start() override;
  #pragma endregion // DrzEngine

    //Graphics impl.
  #pragma region IDrzGraphics
    void DrawPixel(int x, int y, Color color) override;
  #pragma endregion // IDrzGraphics

  private:
    PixelGameEngineApp* pge;
    int width;
    int height;
    int pixelSize;
};

} // namespace drz