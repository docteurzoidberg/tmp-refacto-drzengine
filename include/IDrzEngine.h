#pragma once

#include <cstdint>

namespace drz {

/**
 * @brief IDrzEngine interface for DrzEngine platform libraries
 * 
 */
class IDrzEngine
{
  public:
    virtual ~IDrzEngine() = default;
    virtual void Setup() = 0;
    virtual void Start() = 0;

    virtual uint32_t Now() = 0;
    virtual float GetRandomFloat() = 0;
};

} // namespace drz