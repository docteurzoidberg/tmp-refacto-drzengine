#include <DrzEngine.h>

#include <iostream>

using namespace drz;

DrzEngine::DrzEngine() {
  std::cout << "DrzEngine constructor called" << std::endl;  
}

DrzEngine::~DrzEngine() {
  std::cout << "DrzEngine destructor called" << std::endl;
}

void DrzEngine::Set(IDrzEngine* engine) {
  std::cout << "DrzEngine::Set called" << std::endl;
  //set engine
  DrzEngine::engine = engine;
}

void DrzEngine::Setup() {
  std::cout << "DrzEngine::Setup called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  } 
  engine->Setup();
  if(app==nullptr) {
    std::cerr << "App is null" << std::endl;
    return;  
  }
  app->Setup();
}

void DrzEngine::Loop(float elapsedTime) {
  //std::cout << "DrzEngine::Loop called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  }
  //call loop callback
  //loopCallBack(elapsedTime);
  app->Update(elapsedTime);
}

void DrzEngine::Start() {
  std::cout << "DrzEngine::Start called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  }
  engine->Start();
}

void DrzEngine::SignalHandler(int signum) {
  std::cout << "Interrupt signal (" << signum << ") received.\n";
  // cleanup and close up stuff here  
  // terminate program  
  isRunning = false;
}
