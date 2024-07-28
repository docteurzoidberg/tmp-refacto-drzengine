#pragma once

#include "DrzEngine.h"

#include <olcPixelGameEngine.h>

namespace drz {


class NothingAudio: IAudio {
  void foo() {}
}

class DrzEngine_PGE : public IDrzGraphics, public IDrzInputs, public IDrzEngine, public olc::PixelGameEngine
{
  public:
    DrzEngine_PGE(int width, int height, int pixelSize);
    ~DrzEngine_PGE();

    bool OnUserCreate() override
    {
      // Called once at the start, so create things here
      std::cout << "PGEapp OnUserCreate called" << std::endl;
      //TODO: call DrzEngine::setup here
      engine.Setup();
      return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
      // Called once per frame
      //std::cout << "PGEapp OnUserUpdate called" << std::endl;
      //call loop callback
      engine.Update(fElapsedTime);
      return true;
    }

    template<typename T>
    void use_app(unque_ptr<Engine> engine) {
      this->engine = std::move(engine);
    }

  private:
    unque_ptr<Engine> engine;
    int width;
    int height;
    int pixelSize;
};

} // namespace drz