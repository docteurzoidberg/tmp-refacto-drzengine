#pragma once

#include "DrzEngine.h"

//TODO: include fbgraphics.h

namespace drz {


class DrzEngine_FB : public IDrzGraphics, public IDrzEngine
{
  public:
    DrzEngine_FB(int width, int height);
    ~DrzEngine_FB();

    void Setup() override;
    void Loop(float elapsedTime) override;
    void Start() override;

  private:
    int width;
    int height;
};

} // namespace drz