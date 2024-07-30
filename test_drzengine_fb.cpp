#include <DrzEngine.h>
#include <DrzEngine_FB.h>

#include "TestApp.h"

#include <memory>
#include <iostream>

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginefb = nullptr;

TestApp* testapp = nullptr;

int main(){

  //Engine implementation
  drzenginefb = std::make_unique<DrzEngine_FB>(320, 240);
  testapp = new TestApp();

  DrzEngine::UseApp(testapp);

  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}
