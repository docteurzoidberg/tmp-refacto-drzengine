#include <DrzGraphics.h>

namespace drz {

  using namespace graphics;

  #pragma region DrzGraphics

  IDrzGraphics* DrzGraphics::Get() {
    return instance;
  } 

  void DrzGraphics::Set(IDrzGraphics* graphics) {
    instance = graphics;
  }

  font* DrzGraphics::GetFont() {
    return currentFont;
  }

  font* DrzGraphics::GetFont(std::string fontName) {
    //look for font in map
    if(fonts.find(fontName) != fonts.end()) {
      return fonts[fontName];
    }
    return nullptr;
  }

  void DrzGraphics::SetFont(font* f) {
    currentFont = f;
  }

  void DrzGraphics::SetFont(std::string fontName) {
    //find font in map
    if(fonts.find(fontName) != fonts.end()) {
      currentFont = fonts[fontName];
    }
  }

  void DrzGraphics::LoadFont(std::string fontName, font* f) {
    //add font to map
    fonts[fontName] = f;
  }

  #pragma endregion DrzGraphics

  #pragma region Sprites

  Sprite::Sprite() {

  }

  Sprite::Sprite(int width, int height) {

  }

  Sprite::Sprite(int width, int height, const uint32_t* data) {

  }

  #pragma endregion Sprites

} // namespace drz