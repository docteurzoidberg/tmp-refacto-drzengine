#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzSerial.h>
#include <IDrzSerial.h>

#include "TestApp.h"

#include <memory>

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginepge = nullptr;

TestApp* testapp = nullptr;

IDrzSerial* serial = nullptr;

int main(){

  //Engine implementation
  drzenginepge = std::make_unique<DrzEngine_PGE>(320, 240, 2);

  serial = DrzSerial::Get();


  testapp = new TestApp();

  serial->SetPort("/dev/pts/1");
  serial->SetBaudRate(9600);
  serial->Setup();
  
  DrzEngine::UseApp(testapp);
  
  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Start engine main loop
  DrzEngine::Start();

  return 0;
}
