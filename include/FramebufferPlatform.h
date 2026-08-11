#pragma once

#include <fbgraphics.h>
#include <fbg_fbdev.h>

#include <chrono>
#include <thread>
#include <iostream>
#include <string>

#define OLC_GFX_CUSTOM_EX
#define OLC_PLATFORM_CUSTOM_EX

#include "olcPixelGameEngine.h"

// Custom Platform for Linux Framebuffer using libfbg
class LinuxFBGPlatform : public olc::Platform {
private:
    struct _fbg* fbg_instance = nullptr;
    int width = 0;
    int height = 0;
    const char* fbdev = "/dev/fb0";

    // Initialize framebuffer
    olc::rcode CreateFrameBuffer() {
        fbg_instance = fbg_fbdevSetup(const_cast<char*>(fbdev), 0);
        if (!fbg_instance) return olc::rcode::FAIL;
        width = fbg_instance->width;
        height = fbg_instance->height;
        return olc::rcode::OK;
    }

public:
    // Called once before the engine thread starts
    olc::rcode ApplicationStartUp() override {
        return CreateFrameBuffer();
    }

    // Called once after the engine thread exits
    olc::rcode ApplicationCleanUp() override {
        if (fbg_instance) {
            fbg_close(fbg_instance);
            fbg_instance = nullptr;
        }
        return olc::rcode::OK;
    }

    // Called inside EngineThread() at start
    olc::rcode ThreadStartUp() override {
        return olc::rcode::OK;
    }

    // Called inside EngineThread() at exit
    olc::rcode ThreadCleanUp() override {
        if (olc::renderer) olc::renderer->DestroyDevice();
        return olc::rcode::OK;
    }

    // Create graphics context; called after ThreadStartUp
    olc::rcode CreateGraphics(bool bFullScreen, bool bEnableVSYNC,
                              const olc::vi2d& vViewPos,
                              const olc::vi2d& vViewSize) override {
        if (olc::renderer) {
            olc::renderer->CreateDevice({ this }, bFullScreen, bEnableVSYNC);
            olc::renderer->UpdateViewport(vViewPos, vViewSize);
        }
        return olc::rcode::OK;
    }

    // Tell PGE the "window" (framebuffer) size
    olc::rcode CreateWindowPane(const olc::vi2d& /*vWindowPos*/,
                                olc::vi2d& vWindowSize,
                                bool /*bFullScreen*/) override {
        vWindowSize = { width, height };
        ptrPGE->olc_UpdateWindowSize(width, height);
        return olc::rcode::OK;
    }

    // Optional: set title
    olc::rcode SetWindowTitle(const std::string& s) override {
        std::cout << "PGE Framebuffer: " << s << std::endl;
        return olc::rcode::OK;
    }

    // Main loop: wait until engine stops
    olc::rcode StartSystemEventLoop() override {
        while (ptrPGE->olc_IsRunning()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        return olc::rcode::OK;
    }

    // No input handling
    olc::rcode HandleSystemEvent() override {
        return olc::rcode::OK;
    }

    // Accessors
    struct _fbg* GetFrameBufferInstance() const { return fbg_instance; }
    int GetScreenWidth() const { return width; }
    int GetScreenHeight() const { return height; }
};

// Software Renderer using libfbg
class LinuxFBGRenderer : public olc::Renderer {
private:
    LinuxFBGPlatform* platform = nullptr;
    struct _fbg*      fbg_instance = nullptr;

public:
    // Called from CreateGraphics
    olc::rcode CreateDevice(std::vector<void*> params, bool, bool) override {
        platform     = static_cast<LinuxFBGPlatform*>(params[0]);
        fbg_instance = platform->GetFrameBufferInstance();
        ptrPGE       = platform->ptrPGE;
        return olc::rcode::OK;
    }

    // Teardown
    olc::rcode DestroyDevice() override {
        return olc::rcode::OK;
    }

    // Clear both pages of the flip chain, once.
    // The per-frame clears below are deliberately no-ops, so whatever lies
    // outside the region DisplayFrame() writes (the draw target can be smaller
    // than the framebuffer) has to be blacked out here — in both buffers.
    void PrepareDevice() override {
        if (!fbg_instance) return;
        fbg_clear(fbg_instance, 0);
        fbg_flip(fbg_instance);
        fbg_clear(fbg_instance, 0);
    }

    // No-op on purpose: DisplayFrame() rewrites every pixel it owns, so
    // clearing here is pure memory traffic (~300 KB/frame at 320x240x4) on a
    // platform that has none to spare. Same for ClearBuffer/DrawLayerQuad below
    // — PGE calls all three every frame.
    void PrepareDrawing() override {}

    // Present frame
    void DisplayFrame() override {
        auto spr = ptrPGE->GetDrawTarget();
        for (int y = 0; y < spr->height; ++y) {
            for (int x = 0; x < spr->width; ++x) {
                auto p = spr->GetPixel(x, y);
                fbg_pixela(fbg_instance, x, y, p.b, p.g, p.r, p.a);
            }
        }
        fbg_draw(fbg_instance);
        fbg_flip(fbg_instance);
    }

    // No-op decal mode
    void SetDecalMode(const olc::DecalMode&) override {}

    // Software fallback doesn't update viewport
    void UpdateViewport(const olc::vi2d& /*pos*/, const olc::vi2d& /*size*/) override {}

    // No-op: see PrepareDrawing(). This clears the fbg back buffer, not the PGE
    // draw target, so the app-facing Clear(colour) is unaffected.
    void ClearBuffer(olc::Pixel /*p*/, bool /*bDepth*/) override {}

    // No hardware textures
    uint32_t CreateTexture(uint32_t, uint32_t, bool, bool) override { return 0; }
    void     UpdateTexture(uint32_t, olc::Sprite*) override {}
    void     ReadTexture(uint32_t, olc::Sprite*) override {}
    uint32_t DeleteTexture(uint32_t) override { return 0; }
    void     ApplyTexture(uint32_t) override {}

    // No-op: a hardware renderer blits the layer texture here, but this one has
    // no textures — DisplayFrame() reads the draw target directly. Filling the
    // screen with the layer tint only got overwritten a moment later.
    void DrawLayerQuad(const olc::vf2d& /*offset*/,
                       const olc::vf2d& /*scale*/,
                       const olc::Pixel /*tint*/) override {}

    // Simple decal draw: plot each decal point with texture lookup
    void DrawDecal(const olc::DecalInstance& decal) override {
        if (!decal.decal || !fbg_instance) return;
        auto spr = decal.decal->sprite;
        for (uint32_t i = 0; i < decal.points; ++i) {
            int sx = int(decal.pos[i].x);
            int sy = int(decal.pos[i].y);
            int u  = int(decal.uv[i].x * spr->width);
            int v  = int(decal.uv[i].y * spr->height);
            auto p = spr->GetPixel(u, v);
            // apply tint
            olc::Pixel tp(
                p.r * decal.tint[i].r / 255,
                p.g * decal.tint[i].g / 255,
                p.b * decal.tint[i].b / 255,
                p.a * decal.tint[i].a / 255
            );
            fbg_pixela(fbg_instance, sx, sy, tp.b, tp.g, tp.r, tp.a);
        }
    }
};
