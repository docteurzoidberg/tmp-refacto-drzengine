#include <DrzEngine_FB.h>
#include <iostream>

using namespace drz;

DrzEngine_FB::DrzEngine_FB(int width, int height) : width(width), height(height) {

  std::cout << "DrzEngine_FB constructor called" << std::endl;

  //set engine class
  DrzEngine::Set(this);

  //set graphic class
  DrzGraphics::Set(this);

  //TODO: set input class
  //TODO: set serial class
  //TODO: set audio class
}

DrzEngine_FB::~DrzEngine_FB() {
  std::cout << "DrzEngine_FB destructor called" << std::endl;
}

void DrzEngine_FB::Setup() {
  std::cout << "DrzEngine_FB::Setup called" << std::endl;
}

void DrzEngine_FB::Loop(float elapsedTime) {
  std::cout << "DrzEngine_FB::Loop called" << std::endl;
  //Trigger engine loop
  DrzEngine::Loop(elapsedTime);
}

void DrzEngine_FB::Start() {
  std::cout << "DrzEngine_FB::Start called" << std::endl;
  //TODO open framebuffer
  std::cout << "TODO: setup framebuffer" << std::endl;
}


