#pragma once

#include "IDrzEngineApp.h"
#include "IDrzEngine.h"
#include "IDrzInputs.h"
#include "IDrzSerial.h"
#include "IDrzGraphics.h"
#include "IDrzSam.h"

#include <functional>
#include <memory>

namespace drz 
{

class DrzEngine
{
  public:
    inline static bool isRunning = true;
    
    DrzEngine();
    ~DrzEngine();

    static void Loop(float elapsedTime);
    static void Setup();
    static void Start();

    static void UseApp(IDrzEngineApp* engineapp) {
      app = engineapp;
    }

    static void SignalHandler(int signal);

    static void Set(IDrzEngine* engine);
    static IDrzEngine* Get();

  private:
    inline static IDrzEngineApp* app;
    inline static IDrzEngine* engine;
};

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

class DrzGraphics
{
  public:
    static IDrzGraphics* Get() {
      return instance;
    }
    static void Set(IDrzGraphics* graphics) {
      instance = graphics;
    }

  private:
    inline static IDrzGraphics* instance = nullptr;
};

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

} // namespace drz