#include <DrzGraphics.h>

namespace drz {

  using namespace graphics;

  IDrzGraphics* DrzGraphics::Get() {
    return instance;
  } 

  void DrzGraphics::Set(IDrzGraphics* graphics) {
    instance = graphics;
  }


  #pragma region "Sprites"

    Sprite::Sprite() {

    }

    Sprite::Sprite(int width, int height) {

    }

    Sprite::Sprite(int width, int height, const uint32_t* data) {

    }

  #pragma endregion // Sprites

} // namespace drz