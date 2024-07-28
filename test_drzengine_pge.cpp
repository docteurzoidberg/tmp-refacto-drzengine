#include <DrzEngine.h>
#include <DrzEngine_PGE.h>

#include <memory>
#include <iostream>

using namespace drz;

class Engine {
  IDrzGraphics* graphics;
  IDrzInput* input;
  IDrzAudio* audio;

  virtual void Setup() = 0;
  virtual void Update(float elapsedTime) = 0;
};

//App exemple
class VanAssistant : Engine {
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
  engine = std::make_unique<DrzEngine_PGE>(320, 240, 2);
  engine.use_app(new VanAssistant(engine, engine, engine));
  engine.Start();
  return 0;
}

VanAssistant* app;

void setup() {
  app = new VanAssistant(new graphic_module(), ...);
  app.Setup();
}

viod loop() {
  app.input.update();
  app.Update();
}
