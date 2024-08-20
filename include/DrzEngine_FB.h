/// @file DrzEngine_FB.h
/// @brief DrzEngine_FB class

#pragma once

#include <DrzEngine.h>
#include <DrzGraphics.h>
#include <DrzInputs.h>

#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "olcPixelGameEngine.h"

class LinuxFramebufferPlatform : public olc::Platform {
public:
    int fb_fd = -1;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize;
    char *fb_ptr = nullptr;

    // Platform-specific startup (framebuffer initialization)
    olc::rcode ApplicationStartUp() override {
      fb_fd = open("/dev/fb0", O_RDWR);
      if (fb_fd == -1) {
        perror("Error opening framebuffer device");
        return olc::rcode::FAIL;
      }

      // Get variable screen information
      if (ioctl(fb_fd, FBIOGET_VSCREENINFO, &vinfo)) {
        perror("Error reading variable information");
        return olc::rcode::FAIL;
      }

      // Get fixed screen information
      if (ioctl(fb_fd, FBIOGET_FSCREENINFO, &finfo)) {
        perror("Error reading fixed information");
        return olc::rcode::FAIL;
      }

      // Calculate the size of the screen in bytes
      screensize = vinfo.yres_virtual * finfo.line_length;

      // Map the device to memory
      fb_ptr = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
      if (fb_ptr == MAP_FAILED) {
        perror("Error mapping framebuffer device to memory");
        return olc::rcode::FAIL;
      }

      return olc::rcode::OK;
    }

    // Platform-specific cleanup (release framebuffer resources)
    olc::rcode ApplicationCleanUp() override {
      if (fb_ptr) {
        munmap(fb_ptr, screensize);
        fb_ptr = nullptr;
      }
      if (fb_fd != -1) {
        close(fb_fd);
        fb_fd = -1;
      }
      return olc::rcode::OK;
    }

    // Handle input, typically through `/dev/input/`
    olc::rcode HandleSystemEvent() override {
      // Implement input handling here if necessary
      // This might involve reading from `/dev/input/event*` devices
      return olc::rcode::OK;
    }

    // Return the framebuffer pointer for the renderer to use
    char* GetFramebufferPointer() {
        return fb_ptr;
    }

    long GetFramebufferSize() {
        return screensize;
    }

    fb_var_screeninfo GetScreenInfo() {
        return vinfo;
    }

    fb_fix_screeninfo GetFixedInfo() {
        return finfo;
    }
};

class LinuxFramebufferRenderer : public olc::Renderer {
private:
  LinuxFramebufferPlatform* platform;
  fb_var_screeninfo vinfo;
  fb_fix_screeninfo finfo;
  char *fb_ptr;
  long screensize;

public:
  // Renderer-specific initialization (ties to platform)
  olc::rcode CreateDevice(std::vector<void*> params, bool bFullScreen, bool bVSYNC) override {
    platform = static_cast<LinuxFramebufferPlatform*>(params[0]);
    fb_ptr = platform->GetFramebufferPointer();
    screensize = platform->GetFramebufferSize();
    vinfo = platform->GetScreenInfo();
    finfo = platform->GetFixedInfo();
    return olc::rcode::OK;
  }

  // Renderer-specific cleanup
  olc::rcode DestroyDevice() override {
    // No need for additional cleanup as it is handled by the platform
    return olc::rcode::OK;
  }

  // Prepare the device for rendering (clear framebuffer)
  void PrepareDevice() override {
    if (fb_ptr) {
      memset(fb_ptr, 0, screensize);
    }
  }

  // Present the frame buffer to the screen (finalize the frame)
  void DisplayFrame() override {
    // No need to do anything here as the framebuffer is directly mapped to the screen
  }

  void DrawDecal(const olc::DecalInstance& decal) override {
    olc::Sprite* spr = decal.decal->sprite; // Correct way to access the sprite

    for (int y = 0; y < spr->height; y++) {
      for (int x = 0; x < spr->width; x++) {
        olc::Pixel p = spr->GetPixel(x, y);
        
        // Only draw if pixel is not fully transparent
        if (p.a == 0) continue;

        uint16_t pixel = ((p.r & 0xF8) << 8) | ((p.g & 0xFC) << 3) | (p.b >> 3);
        long location = (x + decal.pos[0].x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                        (y + decal.pos[0].y + vinfo.yoffset) * finfo.line_length;

        if (location >= 0 && location < screensize) {
          *((uint16_t*)(fb_ptr + location)) = pixel;
        }
      }
    }
}

};


#include <chrono>

namespace drz {

class DrzEngine_FB : public IDrzGraphics, public IDrzInputs, public IDrzEngine
{
  public:
    DrzEngine_FB(int width, int height);

    //DrzEngine impl.
    #pragma region IDrzEngine

    ~DrzEngine_FB();
    void Setup() override;
    void Start() override;

    #pragma endregion // DrzEngine

    //Graphics impl.
    #pragma region IDrzGraphics

    bool DrawPixel(int x, int y, Color color) override;
    void DrawLine(int x1, int y1, int x2, int y2, Color color) override;
    void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;
    void DrawText(std::string text, int x, int y, Color color) override;

    void FillCircle(int x, int y, int radius, Color color) override;
    void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, Color color) override;
    void FillRect(int x, int y, int width, int height, Color color) override;

    int GetScreenWidth() override { return width; }
    int GetScreenHeight() override { return height; }

    
    #pragma endregion // IDrzGraphics

  private:
    int width;
    int height;
    
    std::chrono::high_resolution_clock::time_point start;
    std::chrono::high_resolution_clock::time_point lastUpdate;
};

} // namespace drz