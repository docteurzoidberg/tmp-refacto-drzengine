#pragma once

#include "IDrzEngineApp.h"

#include "IDrzEngine.h"
#include "IDrzInputs.h"
#include "IDrzSerial.h"
#include "IDrzSam.h"

namespace drz 
{

#pragma region DrzEngine

/***
 * @brief DrzEngine is the main static engine class
*/
class DrzEngine
{
  public:
    inline static bool isRunning = true;
    
    DrzEngine();
    ~DrzEngine();
    
    static void Set(IDrzEngine* engine);
    static IDrzEngine* Get(); 
    static void SignalHandler(int signal);

    static void Loop(float elapsedTime);
    static void Setup();
    static void Start();

    static void UseApp(IDrzEngineApp* engineapp);
    static bool AppCommand(const std::string& command);

  private:
    inline static IDrzEngineApp* app;
    inline static IDrzEngine* engine;
};

#pragma endregion

#pragma region DrzSerial

/***
 * @brief DrzSerial is the main static serial class
*/
class DrzSerial
{
  public:
    static IDrzSerial* Get() {
      return instance;
    }
    static void Set(IDrzSerial* serial) {
      instance = serial;
    }

  private:
    inline static IDrzSerial* instance = nullptr;
};

#pragma endregion


#pragma region DrzSam
/***
 * @brief DrzSam is the main static sam class
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

#pragma endregion

#pragma region DrzInputs

/***
 * @brief DrzInputs is the main static input class
*/
class DrzInputs {
  public:
    static IDrzInputs* Get() {
      return instance;
    }
    static void Set(IDrzInputs* inputs) {
      instance = inputs;
    }

  private:
    inline static IDrzInputs* instance = nullptr;
};

#pragma endregion
} // namespace drz