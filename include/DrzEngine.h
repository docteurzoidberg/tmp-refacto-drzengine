#pragma once

#include "IDrzEngineApp.h"
#include "IDrzEngine.h"

namespace drz 
{
/***
 * @brief DrzEngine is the main static engine class to acces IDrzEngine and IDrzEngineApp implementations
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

    static uint32_t Now();

    static void Loop(float elapsedTime);
    static void Setup();
    static void Start();

    static void UseApp(IDrzEngineApp* engineapp);
    static bool AppCommand(const std::string& command);

  private:
    inline static IDrzEngineApp* app;
    inline static IDrzEngine* engine;
};

} // namespace drz