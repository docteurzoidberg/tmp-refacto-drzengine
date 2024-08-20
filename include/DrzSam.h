/// @file DrzSam.h
/// @brief DrzSam class

#pragma once

#include <IDrzSam.h>

namespace drz {

/***
 * @brief DrzSam is the main static sam class to acces an IDrzSam implementation
*/
class DrzSam
{
  public:
    static IDrzSam* Get() {
      return instance;
    }
    static void Set(IDrzSam* sam) {
      instance = sam;
    }

  private:
    inline static IDrzSam* instance = nullptr;
};  

} // namespace drz