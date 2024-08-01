#pragma once

#include <IDrzEngine.h>

#include <DrzInputs.h>
#include <DrzGraphics.h>
#include <DrzEngine.h>


#include <olcPixelGameEngine.h>

namespace drz {


/***
 * @brief PixelGameEngineApp is a wrapper for olc::PixelGameEngine
*/
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
      // Q: exit the app
      if(GetKey(olc::Key::Q).bPressed) {
        exit(0);
      }

      // F1: show console
      if(GetKey(olc::Key::F1).bPressed) {
        ConsoleShow(olc::Key::ESCAPE, false);
      }

      // Called once per frame
      DrzEngine::Loop(fElapsedTime);
      return true;
    }

    bool OnUserDestroy() override
    {
      // Called once at the end, so clean up here
      DrzEngine::isRunning = false;
      return true;
    }

    bool OnConsoleCommand(const std::string& command) override
    {
      return DrzEngine::AppCommand(command);
    }
};


/***
 * @brief DrzEngine_PGE is a olc::PixelGameEngine implementation of DrzEngine
*/
class DrzEngine_PGE : public IDrzGraphics, public IDrzInputs, public IDrzEngine
{
  public:
    DrzEngine_PGE(int width, int height, int pixelSize);

    //Engine impl.-----------------------------------------------
    #pragma region IDrzEngine

    ~DrzEngine_PGE();
    void Setup() override;
    void Start() override;

    #pragma endregion // DrzEngine

    //Graphics impl.---------------------------------------------
    #pragma region IDrzGraphics

    void Clear(Color color) override;

    void LoadFont(std::string fontName, font* f) override;
    void SetFont(std::string fontName) override;

    bool DrawPixel(int x, int y, Color color) override;
    void DrawLine(int x1, int y1, int x2, int y2, Color color) override;
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;
    void DrawText(std::string text, int x, int y, Color color) override;
    void DrawRect(int x, int y, int width, int height, Color color) override;
    void DrawPartialSprite(int32_t x, int32_t y, Sprite* sprite, int32_t ox, int32_t oy, int32_t w, int32_t h) override;
    void DrawPartialSprite(vi2d pos, Sprite* sprite, vi2d srcPos, vi2d size) override;

    void FillRect(int x, int y, int width, int height, Color color) override;
    void FillCircle(int x, int y, int radius, Color color) override;
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;

    int GetScreenWidth() override { return width; }
    int GetScreenHeight() override { return height; }

    float GetRandomFloat() override;

    #pragma endregion // IDrzGraphics

    //Inputs impl.-----------------------------------------------
    #pragma region IDrzInputs

    HardwareButton GetKey(Key key) override;

    #pragma endregion // IDrzInputs

  private:
    PixelGameEngineApp* pge;
    int width;
    int height;
    int pixelSize;
};

} // namespace drz