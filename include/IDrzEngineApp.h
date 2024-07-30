#pragma once

namespace drz {

class IDrzEngineApp
{
  public:
    virtual ~IDrzEngineApp() = default;
    virtual void Setup() = 0;
    virtual void Update(float elapsedTime) = 0;

};

} // namespace drz