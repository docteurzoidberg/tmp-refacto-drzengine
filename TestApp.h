#pragma once

#include <DrzEngine.h> 
#include <DrzGraphics.h>

#include <iostream>

using namespace drz;

//App exemple
class TestApp: public IDrzEngineApp {
  public:
    TestApp();
    ~TestApp(); 
    void Setup();
    void Update(float elapsedTime);
};
