#include <DrzInputs.h>

namespace drz {

  using namespace inputs;

  #pragma region DrzInputs

  IDrzInputs* DrzInputs::Get() {
    return instance;
  } 

  void DrzInputs::Set(IDrzInputs* inputs) {
    instance = inputs;
  }

  #pragma endregion DrzInputs

} // namespace drz