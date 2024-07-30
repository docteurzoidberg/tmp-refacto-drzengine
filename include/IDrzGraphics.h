#pragma once

#include <DrzEngine.h>
#include <DrzGraphics.h>

namespace drz {

  using namespace graphics;

  class IDrzGraphics
  {
    public: 
      virtual void DrawPixel(int x, int y, Color color) = 0;
  };

} // namespace

