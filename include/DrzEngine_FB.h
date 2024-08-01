#pragma once

#include "DrzEngine.h"
#include "DrzGraphics.h"

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

    bool DrawPixel(int x, int y, Color color) override;
    void DrawLine(int x1, int y1, int x2, int y2, Color color) override;
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;
    void DrawText(std::string text, int x, int y, Color color) override;

    void FillCircle(int x, int y, int radius, Color color) override;
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;
    void FillRect(int x, int y, int width, int height, Color color) override;

    int GetScreenWidth() override { return width; }
    int GetScreenHeight() override { return height; }

    
    #pragma endregion // IDrzGraphics

  private:
    int width;
    int height;
    
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point lastUpdate;
};

} // namespace drz