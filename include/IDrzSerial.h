#pragma once

namespace drz {

/**
 * @brief IDrzSerial interface
 * 
 */
class IDrzSerial {
  public:
    virtual void SetPort(const char* port) = 0;
    virtual void SetBaudRate(int baudrate) = 0;
    virtual bool Setup() = 0;
    virtual int Read() = 0;
    virtual bool Write(const char* data, int length) = 0;
    virtual bool Close() = 0;
    virtual char* GetReadBuffer() = 0;
};

} // namespace

