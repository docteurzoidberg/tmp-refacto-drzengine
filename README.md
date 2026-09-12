# DrzEngine

Static C++20 library providing a portable engine layer — graphics, inputs, serial, widgets,
display pages, a small 3D pipeline — on top of a swappable platform backend.

The only backend today is [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine)
(`DrzEngine_PGE`), built either against X11/OpenGL or against the **Linux framebuffer** via a
custom PGE platform + renderer. The target hardware is small and slow: Raspberry Pi Zero,
320x240-ish screens, buildroot rootfs.

> **Status: pre-release, extraction in progress.** DrzEngine is the reusable part being pulled
> out of `vanassistant`, which is still its only consumer. Version `0.1.0` is tagged,
> the public API is not stable yet, and there is no automated test suite.
> See [CHANGELOG.md](CHANGELOG.md) for what has landed and what is known broken.

## Requirements

- CMake ≥ 3.5, a C++20 compiler (GCC or Clang) and a C99 compiler
- **libpng** — needed by both backends. PGE auto-selects its libpng image loader on Linux and
  `src/DrzEngine_PGE.cpp` is the translation unit that compiles it. `drzenginepgefb` declares
  the dependency itself, so a cross-compile sysroot without libpng fails at configure time
  rather than at the consumer's link step. Define `OLC_IMAGE_HEADLESS` if a build genuinely
  needs no image loading.
- **OpenGL (GLVND), X11, pthread** — for the X11 backend: the test app links them, and so must
  any executable built on `drzenginepge` (see [Using it in a project](#using-it-in-a-project))
- Doxygen — optional, only for `BUILD_DOC=ON`

## Build

Out of tree, **one configure per backend** (see the `USE_FBGPLATFORM` warning below):

```bash
# X11 / OpenGL library + test app
cmake -S . -B build -DBUILD_DOC=OFF -DBUILD_PGEFBLIB=OFF
cmake --build build -j$(nproc)          # -> build/libdrzenginepge.a, build/test_drzengine_pge

# Linux framebuffer library (fbgraphics), no X11 needed
cmake -S . -B build.fb -DBUILD_DOC=OFF -DBUILD_PGELIB=OFF -DBUILD_TEST_APPS=OFF
cmake --build build.fb -j$(nproc)       # -> build.fb/libdrzenginepgefb.a
```

`cmake --preset default` also works (Unix Makefiles, Debug, `binaryDir = build/`) but leaves
every option at its default, i.e. everything ON — which is exactly the case to avoid.

> ⚠️ **Never enable `BUILD_PGELIB` and `BUILD_PGEFBLIB` in the same build directory.**
> `add_compile_definitions(USE_FBGPLATFORM)` sits inside the `BUILD_PGEFBLIB` block but is
> directory-scoped, so with both options ON `drzenginepge` is *also* compiled with the
> framebuffer platform: you get a library named "pge" that is really an FB build, silently and
> without a warning.

### Options

| Option | Default | Effect |
| --- | --- | --- |
| `BUILD_PGELIB` | ON | Build `drzenginepge` (X11/OpenGL backend) |
| `BUILD_PGEFBLIB` | ON | Build `drzenginepgefb` (Linux framebuffer backend, needs libpng) |
| `BUILD_TEST_APPS` | ON | Build `test_drzengine_pge` / `test_drzengine_pgefb` |
| `BUILD_DOC` | ON | Generate Doxygen docs; only warns if Doxygen is missing |
| `DRZENGINE_OPTIMIZE` | ON | Keep the hot paths at `-O2` even in Debug/unset build types |

`DRZENGINE_OPTIMIZE` exists because a consumer using `add_subdirectory()` picks the build type
for everyone: a Debug app would otherwise compile the whole rasteriser, text rendering and
widgets at `-O0` on the Pi. Turn it off to step through the rasteriser.

### Test apps

`test_drzengine_pge` and `test_drzengine_pgefb` build the same sources against their respective
backend. They are **manual smoke apps**, not a test suite: a 320x240 surface at pixel size 2
(an X11 window, or the framebuffer), `Q` quits, `F1` opens the PGE console. They call
`serial->Setup()` on `/dev/pts/1` and will fail there unless a pty exists — expected when you
only want to check that things link.

## Using it in a project

Add the repo as a subdirectory and pin every option before `add_subdirectory()`, keeping exactly
one backend enabled:

```cmake
cmake_policy(SET CMP0077 NEW)   # so the set() calls below win over the option() defaults

set(BUILD_PGELIB ON)            # or BUILD_PGEFBLIB for the framebuffer build
set(BUILD_PGEFBLIB OFF)
set(BUILD_DOC OFF)
set(BUILD_TEST_APPS OFF)
add_subdirectory(/path/to/drzengine ${CMAKE_BINARY_DIR}/drzengine)

target_link_libraries(myapp PRIVATE drzenginepge)

# drzenginepge does not declare PGE's own dependencies — link them yourself
find_package(PNG REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(Threads REQUIRED)
target_link_libraries(myapp PRIVATE PNG::PNG OpenGL::GL X11::X11 Threads::Threads)
```

Both library targets export `include/` as a `PUBLIC` include directory, so public headers are
included angle-style: `#include <DrzGraphics.h>`. `examples/project_template/` holds a
(currently incomplete) skeleton.

The two backends differ here: **`drzenginepgefb` declares its libpng dependency itself** and
consumers get it transitively, while **`drzenginepge` declares nothing** — an executable
linking it needs libpng, OpenGL, X11 and pthread of its own, otherwise the link fails with
~180 undefined `png_*` / `gl*` / `X*` symbols.

The library builds at C++20, but its **public headers are kept C++17-clean** because
vanassistant builds at C++17.

## Quick start

```cpp
#include <DrzEngine.h>
#include <DrzEngine_PGE.h>
#include <DrzGraphics.h>

using namespace drz;

class MyApp : public IDrzEngineApp {
public:
  void Setup() override {}
  void Update(float dt) override {
    DrzGraphics::Get()->Clear(Color(0, 0, 0));
    DrzGraphics::Get()->FillCircle(160, 120, 20, Color(255, 0, 0));
  }
  bool Command(const std::string& c) override { return false; }
  bool Data(const uint8_t* d, size_t n) override { return false; }
};

int main() {
  new DrzEngine_PGE(320, 240, 2);   // registers itself into the facades
  auto* app = new MyApp();          // must come *after* the backend
  DrzEngine::UseApp(app);
  DrzEngine::Setup();
  DrzEngine::Start();               // blocks until the window closes
}
```

**Construction order matters.** `DrzEngine_PGE` registers itself into the static facades from
its own constructor (`DrzEngine::Set(this)`, `DrzGraphics::Set(this)`, `DrzInputs::Set(this)`),
so anything calling `DrzGraphics::Get()` in *its* constructor — `Widget`, most apps — has to be
created after the backend. Serial follows the same pattern: `new DrzSerial_Linux();` with its
result discarded is intentional, the object is reachable through `DrzSerial::Get()`.

Text draws nothing until the app has called `LoadFont()` then `SetFont()`.

## Architecture

Everything follows one pattern: a pure interface `IDrzX`, plus a static facade `DrzX` holding a
single `IDrzX*` set at startup.

| Facade | Interface | Implementation |
| --- | --- | --- |
| `DrzEngine` | `IDrzEngine` | `DrzEngine_PGE` |
| `DrzGraphics` | `IDrzGraphics` | `DrzEngine_PGE` |
| `DrzInputs` | `IDrzInputs` | `DrzEngine_PGE` |
| `DrzSerial` | `IDrzSerial` | `DrzSerial_Linux`, `DrzSerial_Null` (header-only) |
| `DrzSam` | `IDrzSam` | provided by the app (speech synthesis) |

`DrzEngine_PGE` implements three of those interfaces at once and picks the serial
implementation for the platform: `DrzSerial_Linux` on Linux, `DrzSerial_Null` (no-op) on
emscripten.

`DrzGraphics` is **not** a pure passthrough — it owns the font map, the text cursor, the text
colours and the whole Adafruit-GFX-style glyph rasteriser, and draws text by calling back into
the backend's `DrawPixel()`. That is why `DrzEngine_PGE::DrawText` delegates to
`DrzGraphics::DrawText` rather than to `olc::PixelGameEngine::DrawString`.

### Text

`DrawText(text, x, y, color)` takes the **top-left of the text's line box**, not the baseline
as Adafruit GFX does: `DrawText("A", 0, 0)` is fully visible in the top-left corner, and the
vertical position depends on the font only, never on the string. `GetTextBounds()` returns the
ink rectangle for the same `(x, y)`, `GetFontAscent()` the top-to-baseline distance. The full
contract, with the alignment recipes, is in [docs/text-rendering.md](docs/text-rendering.md).

### Frame lifecycle

```
main()
  new DrzEngine_PGE(w, h, pixelSize)   // registers itself into the facades
  new App()                            // implements IDrzEngineApp
  DrzEngine::UseApp(app)
  DrzEngine::Setup()   -> engine->Setup() (pge->Construct) then app->Setup()
  DrzEngine::Start()   -> engine->Start() -> pge->Start()   // blocks
                            PixelGameEngineApp::OnUserUpdate(dt)
                              -> DrzEngine::Loop(dt) -> app->Update(dt)
                            OnConsoleCommand -> DrzEngine::AppCommand -> app->Command
```

Rendering is the app's job inside `Update()`; there is no separate render callback at engine
level.

### UI layer

`DisplayPageManager` is optional: a static list of `DisplayPage*`, each owning `Widget*`s. The
app calls `DisplayPageManager::ReadInputs/Update/Render` from its own `Update()`, and pages
navigate with `GoToPage(name)` / `Back()`. A `Widget` draws in widget-local coordinates and
offsets by `screenX`/`screenY`.

### 3D

`drz::GFX3D` (`include/gfx3d.h`) is a port of `olcPGEX_Graphics3D`: `GFX3D::Math`
(`mat4x4`, `vec3d`), `PipeLine` (projection, camera, transform, texture, 4 light slots,
near-plane and viewport clipping) and triangle rasterisers backed by a depth buffer. It renders
through `IDrzGraphics` only — call `GFX3D::ConfigureDisplay(gfx)` once, after the backend exists
and before `ClearDepth()`/`Render()`.

`DrzGraphics.h` also carries an older, thinner `drz::graphics::Vector` / `Matrix4x4` / `Model`
set with different conventions. Prefer `GFX3D` for new code, and don't mix the two `vec3d`
types without converting explicitly.

## Layout

```
include/            public headers (added to the include path of every consumer)
  IDrz*.h           interfaces
  Drz*.h            static facades
  DrzEngine_PGE.h   PGE backend
  FramebufferPlatform.h  olc::Platform + olc::Renderer over lib/fb
  DrzGraphics.h     Color/Sprite/font/rect/vec + IDrzGraphics + DrzGraphics
  gfx3d.h           GFX3D 3D pipeline
  olcPixelGameEngine.h   vendored third party — do not edit
src/                implementations
lib/fb/             vendored fbgraphics (C) — do not edit
examples/project_template/   skeleton for a downstream project
docs/Doxyfile.in    used when BUILD_DOC=ON
TestApp.{h,cpp}, test_drzengine_pge.cpp   manual smoke app
```

## Third party

- [olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) v2.25 — OLC-3,
  © OneLoneCoder.com. Vendored as `include/olcPixelGameEngine.h`.
- [fbgraphics](https://github.com/grz0zrg/fbg) — BSD-3-Clause, © Julien Verneuil. Vendored
  under `lib/fb/`.

Neither is edited or reformatted here.
