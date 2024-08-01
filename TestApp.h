#pragma once

#include <DrzGraphics.h>

#include <cstdint>
#include <string>

class IDrzEngineApp {
  public:
    virtual ~IDrzEngineApp() {}
    virtual void Setup() = 0;
    virtual void Update(float elapsedTime) = 0;
    virtual bool Command(const std::string& command) = 0;
    virtual bool Data(const uint8_t* data, size_t size) = 0;
};

class IDrzGraphics;

//App exemple

namespace drz {

class TestApp: public IDrzEngineApp {
  public:
    TestApp();
    ~TestApp(); 
    void Setup();
    void Update(float elapsedTime);
    bool Command(const std::string& command);
    bool Data(const uint8_t* data, size_t size);
    IDrzGraphics* gfx;
};

} // namespace drz