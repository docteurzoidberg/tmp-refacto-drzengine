#pragma once

#include "IDrzEngine.h"
#include "IDrzInputs.h"
#include "IDrzSerial.h"
#include "IDrzGraphics.h"
#include "IDrzSam.h"

#include <functional>

namespace drz 
{

///Input keys (taken from olcPixelGameEngine.h)
enum Key {
  NONE,
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  K0, K1, K2, K3, K4, K5, K6, K7, K8, K9,
  F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
  UP, DOWN, LEFT, RIGHT,
  SPACE, TAB, SHIFT, CTRL, ALT, INS, DEL, HOME, END, PGUP, PGDN,
  BACK, ESCAPE, RETURN, ENTER, PAUSE, SCROLL,
  NP0, NP1, NP2, NP3, NP4, NP5, NP6, NP7, NP8, NP9,
  NP_MUL, NP_DIV, NP_ADD, NP_SUB, NP_DECIMAL, PERIOD,
  EQUALS, COMMA, MINUS,
  OEM_1, OEM_2, OEM_3, OEM_4, OEM_5, OEM_6, OEM_7, OEM_8,
  CAPS_LOCK, ENUM_END
};

struct HardwareButton
{
  bool isPressed;
  bool isReleased;
  bool isHeld;
};

class DrzEngine
{
  public:
    DrzEngine();
    ~DrzEngine();


    static void Loop(float elapsedTime);
    static void Setup();
    static void Start();
    static void SetLoopCallBack(std::function<void(float)> loopCallBack) {
      DrzEngine::loopCallBack = loopCallBack;
    }

    static void Set(IDrzEngine* engine);
    static IDrzEngine* Get();

  //using IDrzInputs
    void GetKey(Key k);
  //using IDrzSerial
    void ReadSerial();
  //using IDrzGraphics
    //...
  private:
    inline static IDrzEngine* engine;
    inline static IDrzInputs* inputs;
    inline static IDrzSerial* serial;
    inline static IDrzGraphics* graphics;
    inline static IDrzSam* sam;
    inline static std::function<void(float)> loopCallBack = [](float elapsedTime) {
      return;
    };
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