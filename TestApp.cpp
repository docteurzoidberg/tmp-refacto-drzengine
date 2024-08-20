#include "TestApp.h"

#include <iostream>

using namespace drz;

TestApp::TestApp() {
  std::cout << "TestApp constructor called" << std::endl;
  gfx = DrzGraphics::Get();
}

TestApp::~TestApp() {
  std::cout << "TestApp destructor called" << std::endl;
}

void TestApp::Setup() {
  std::cout << "TestApp::Setup called" << std::endl;
  //TODO: setup display pages
  //DisplayPage * roadPage = new RoadPage();
  //DrzEngine::AddDisplayPage(roadPage);
  //DrzEngine::SetCurrentDisplayPage(roadPage);
}

void TestApp::Update(float elapsedTime) {
  std::cout << "TestApp::Update called" << std::endl;

  //Draw pixels using graphics
  //auto gfx = DrzGraphics::Get();
  gfx->DrawPixel(0, 0, Color(255, 0, 0));
}

bool TestApp::Command(const std::string& command) {
  std::cout << "TestApp::Command called" << std::endl;
  std::cout << "Command: " << command << std::endl;
  return false;
}

bool TestApp::Data(const uint8_t* data, size_t size) {
  std::cout << "TestApp::Data called" << std::endl;
  std::cout << "Data size: " << size << std::endl;
  return false;
}