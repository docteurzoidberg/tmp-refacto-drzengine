/// @file DrzSerial_Null.h
/// @brief DrzSerial_Null class

#pragma once

#include <string.h>

#include <IDrzSerial.h>
#include <DrzSerial.h>

namespace drz {

/***
 * @brief DrzSerial_Null is a no-op IDrzSerial implementation for the platforms
 *        that have no serial port at all (WASM/emscripten). It lets the engine
 *        keep a valid DrzSerial::Get() instead of a null instance: reads always
 *        report "no byte available" and writes are silently discarded.
*/
class DrzSerial_Null: public IDrzSerial {

  public:

    // Same size as the Linux implementation so callers can share their parsing code
    char read_buf[1024];

    DrzSerial_Null() {
      memset(&read_buf, '\0', sizeof(read_buf));
      DrzSerial::Set(this);
    }

    ~DrzSerial_Null() {
      Close();
    }

    void SetPort(const char* port) override {
      // no serial port on this platform
    }

    void SetBaudRate(int baudrate) override {
      // no serial port on this platform
    }

    bool Setup() override {
      return true;
    }

    int Read() override {
      // 0 = no byte received, which is a valid non-error result for the callers
      return 0;
    }

    bool Write(const char* data, int len) override {
      // discarded
      return true;
    }

    char* GetReadBuffer() override {
      return read_buf;
    }

    bool Close() override {
      return true;
    }
};

} // namespace drz
