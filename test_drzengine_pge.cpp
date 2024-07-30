#include <DrzEngine.h>
#include <DrzEngine_PGE.h>

#include "TestApp.h"

#include <memory>
#include <iostream>

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginepge = nullptr;

TestApp* testapp = nullptr;

int main(){

  //Engine implementation
  drzenginepge = std::make_unique<DrzEngine_PGE>(320, 240, 2);

  testapp = new TestApp();
  
  DrzEngine::UseApp(testapp);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}
