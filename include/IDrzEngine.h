#pragma once

namespace drz {

class IDrzEngine
{
  public:
    virtual ~IDrzEngine() = default;
    virtual void Setup() = 0;
    virtual void Start() = 0;
};

} // namespace drz