#include "TestApp.h"

TestApp::TestApp() {
  std::cout << "VanAssistant constructor called" << std::endl;
}

TestApp::~TestApp() {
  std::cout << "VanAssistant destructor called" << std::endl;
}

void TestApp::Setup() {
  std::cout << "VanAssistant::Setup called" << std::endl;
  //TODO: setup display pages
  //DisplayPage * roadPage = new RoadPage();
  //DrzEngine::AddDisplayPage(roadPage);
  //DrzEngine::SetCurrentDisplayPage(roadPage);
}

void TestApp::Update(float elapsedTime) {
  std::cout << "VanAssistant::Update called" << std::endl;

  //Draw pixels using graphics
  auto gfx = DrzGraphics::Get();
  gfx->DrawPixel(0, 0, Color(255, 0, 0));
}
