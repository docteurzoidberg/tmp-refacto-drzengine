#pragma once

#include <IDrzSerial.h>

namespace drz {

  /**
   * @brief DrzSerial is the main static serial class to access an IDrzSerial implementation
   */
  class DrzSerial {
  public:
    static IDrzSerial* Get() {
      return instance;
    }

    static void Set(IDrzSerial* serial) {
      instance = serial;
    }

  protected:
    inline static IDrzSerial* instance = nullptr;
  };
} // namespace drz