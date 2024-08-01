#pragma once

#include <string>
#include <cstdint>

namespace drz {

class IDrzEngineApp
{
  public:
    virtual ~IDrzEngineApp() = default;
    virtual void Setup() = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual bool Command(const std::string& command) = 0;
    virtual bool Data(const uint8_t* data, size_t size) = 0;
};

} // namespace drz