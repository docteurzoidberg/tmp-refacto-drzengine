#include <DrzEngine.h>

#include <iostream>

using namespace drz;

DrzEngine::DrzEngine() {
  std::cout << "DrzEngine constructor called" << std::endl;  
}

DrzEngine::~DrzEngine() {
  std::cout << "DrzEngine destructor called" << std::endl;
}

void DrzEngine::GetKey(Key k) {
  std::cout << "GetKey called with key: " << k << std::endl;
}

void DrzEngine::ReadSerial() {
  std::cout << "ReadSerial called" << std::endl;
  //auto serial = DrzSerial::Get();
}

void DrzEngine::Set(IDrzEngine* engine) {
  std::cout << "DrzEngine::Set called" << std::endl;
  //set engine
  DrzEngine::engine = engine;
}

void DrzEngine::Loop(float elapsedTime) {
  //std::cout << "DrzEngine::Loop called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  }
  //call loop callback
  loopCallBack(elapsedTime);
}

void DrzEngine::Setup() {
  std::cout << "DrzEngine::Setup called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  }
  engine->Setup();
}

void DrzEngine::Start() {
  std::cout << "DrzEngine::Start called" << std::endl;
  if(engine==nullptr) {
    std::cerr << "Engine is null" << std::endl;
    return;
  }
  engine->Start();
}
