#include <DrzEngine.h>
#include <DrzEngine_FB.h>

#include <memory>
#include <iostream>

using namespace drz;

std::unique_ptr<IDrzEngine> drzenginefb = nullptr;

//App exemple
class VanAssistant {
  public:
    VanAssistant() {
      std::cout << "VanAssistant constructor called" << std::endl;
    }

    ~VanAssistant() {
      std::cout << "VanAssistant destructor called" << std::endl;
    }

    void Setup() {
      std::cout << "VanAssistant::Setup called" << std::endl;
      //TODO: setup display pages
      //DisplayPage * roadPage = new RoadPage();
      //DrzEngine::AddDisplayPage(roadPage);
      //DrzEngine::SetCurrentDisplayPage(roadPage);
    }

    void Update(float elapsedTime) {
      //std::cout << "VanAssistant::Update called" << std::endl;
    }
};

VanAssistant* vanassistant = nullptr;

void drzLoop(float elapsedTime) {
  vanassistant->Update(elapsedTime);
}

int main(){

  //Engine implementation
  drzenginefb = std::make_unique<DrzEngine_FB>(320, 240);

  //engine setup autodetects engine sub parts
  DrzEngine::Setup();  

  //Engine must be setup before vanassistant so vanassistant can use DrzSerial, DrzGraphics, DrzInputs, etc
  vanassistant = new VanAssistant();
  vanassistant->Setup();

  //Start engine main loop
  DrzEngine::SetLoopCallBack(drzLoop);
  DrzEngine::Start();

  return 0;
}
