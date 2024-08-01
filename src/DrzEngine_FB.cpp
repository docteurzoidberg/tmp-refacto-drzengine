#include <DrzEngine_FB.h>
#include <iostream>
#include <signal.h>

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

void DrzEngine_FB::Start() {
  std::cout << "DrzEngine_FB::Start called" << std::endl;

  //sigint handler
  signal(SIGINT, DrzEngine::SignalHandler);

  //TODO open framebuffer
  std::cout << "TODO: setup framebuffer" << std::endl;
  //keep elapsing time
  float elapsedTime = 0.0f; 

  start = std::chrono::high_resolution_clock::now();
  lastUpdate = start;

  while(DrzEngine::isRunning) {
    //calculate elapsed time in milliseconds
    elapsedTime = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - lastUpdate).count() / 1000.0f;
    DrzEngine::Loop(elapsedTime);
    lastUpdate = std::chrono::high_resolution_clock::now();
  }
}

bool DrzEngine_FB::DrawPixel(int x, int y, Color color) {
  //TODO
  return true;
}

void DrzEngine_FB::DrawLine(int x1, int y1, int x2, int y2, Color color) {
  //TODO
}

void DrzEngine_FB::DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  //TODO
}

void DrzEngine_FB::DrawText(std::string text, int x, int y, Color color) {
  //TODO
}

void DrzEngine_FB::FillCircle(int x, int y, int radius, Color color) {
  //TODO
}

void DrzEngine_FB::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) {
  //TODO
}



