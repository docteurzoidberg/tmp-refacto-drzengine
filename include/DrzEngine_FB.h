#pragma once

#include "DrzEngine.h"

#include <chrono>

//TODO: include fbgraphics.h

namespace drz {

class DrzEngine_FB : public IDrzGraphics, public IDrzEngine
{
  public:
    DrzEngine_FB(int width, int height);

    //DrzEngine impl.
    #pragma region IDrzEngine
    ~DrzEngine_FB();
    void Setup() override;
    void Start() override;
    #pragma endregion // DrzEngine

    //Graphics impl.
    #pragma region IDrzGraphics
    void DrawPixel(int x, int y, Color color) override;
    #pragma endregion // IDrzGraphics

  private:
    int width;
    int height;
    
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point lastUpdate;
};

} // namespace drz