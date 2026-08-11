# DrzEngine

Static C++20 library providing a portable engine layer — graphics, inputs, serial, widgets,
display pages — on top of a swappable platform backend. The only backend today is
[olcPixelGameEngine](https://github.com/OneLoneCoder/olcPixelGameEngine) (`DrzEngine_PGE`),
built either against X11/OpenGL or against the Linux framebuffer via a custom PGE
platform + renderer.

Target hardware is small and slow (Raspberry Pi Zero, 320x240-ish screens, buildroot
rootfs). The real consumer is `/home/drzoid/dev/vanassistant`, which pulls this repo in
with `add_subdirectory()` — see *Consumer* below. `README.md` still says
"tmp-refacto-drzengine": this repo is the reusable part being extracted from vanassistant,
and the extraction is not finished.

## Build

Out-of-tree, one configure per backend (see the `USE_FBGPLATFORM` gotcha — do **not**
enable both libs in a single configure):

```bash
# X11 / OpenGL library + test app
cmake -S . -B build -DBUILD_DOC=OFF -DBUILD_PGEFBLIB=OFF
cmake --build build -j$(nproc)          # -> build/libdrzenginepge.a, build/test_drzengine_pge

# Linux framebuffer library (fbgraphics), no X11 needed
cmake -S . -B build.fb -DBUILD_DOC=OFF -DBUILD_PGELIB=OFF -DBUILD_TEST_APPS=OFF
cmake --build build.fb -j$(nproc)       # -> build.fb/libdrzenginepgefb.a
```

`cmake --preset default` also works (Unix Makefiles, Debug, `binaryDir = build/`) but keeps
every option at its default (everything ON).

Options: `BUILD_PGELIB`, `BUILD_PGEFBLIB`, `BUILD_DOC`, `BUILD_TEST_APPS` — all default ON.
`DRZENGINE_OPTIMIZE` (default ON) keeps the hot paths at `-O2` even when the build type is
Debug or unset, because a consumer's Debug build would otherwise compile the whole
rasteriser at `-O0`; turn it off to step through the rasteriser.
`BUILD_DOC` only warns when Doxygen is missing. Test apps additionally need
OpenGL (GLVND), X11 and pthread.

`BUILD_PGEFBLIB` requires libpng at configure time: PGE auto-selects its libpng image
loader on Linux and compiles it into `src/DrzEngine_PGE.cpp`, so `drzenginepgefb` links
`PNG::PNG`. A cross-compile sysroot without libpng now fails at configure instead of at the
consumer's link step. If a build genuinely needs no image loading, defining
`OLC_IMAGE_HEADLESS` drops the loader and the dependency with it.

There is no test suite. `test_drzengine_pge` is a manual smoke app (opens a 320x240 window
at pixel size 2, `Q` quits, `F1` opens the PGE console). It calls `serial->Setup()` on
`/dev/pts/1` and will fail there unless a pty exists — that is expected when you only want
to check that things link.

`build/` and `.cache/` are gitignored; prefer building into the scratchpad or `build.*/`
when you just need a compile check, so the user's `build/` and `compile_commands.json`
(used by clangd, see `.vscode/settings.json`) stay intact.

## Architecture

Everything is wired through the same pattern: a pure interface `IDrzX`, plus a static
facade `DrzX` holding a single `IDrzX*` set at startup.

| Facade | Interface | Implementation |
| --- | --- | --- |
| `DrzEngine` | `IDrzEngine` | `DrzEngine_PGE` |
| `DrzGraphics` | `IDrzGraphics` | `DrzEngine_PGE` |
| `DrzInputs` | `IDrzInputs` | `DrzEngine_PGE` |
| `DrzSerial` | `IDrzSerial` | `DrzSerial_Linux` (header-only) |
| `DrzSam` | `IDrzSam` | provided by the app (speech synthesis) |

`DrzEngine_PGE` implements three of those interfaces at once and **registers itself from
its own constructor** (`DrzEngine::Set(this); DrzGraphics::Set(this); DrzInputs::Set(this);`).
`DrzSerial_Linux` does the same (`DrzSerial::Set(this)`), which is why
`new DrzSerial_Linux();` appears with its result discarded — the object is reachable through
the facade, intentionally leaked.

Consequence: **construction order matters**. Anything calling `DrzGraphics::Get()` in its
constructor (e.g. `Widget`, `TestApp`) must be created *after* the backend object.

`DrzGraphics` is not a pure passthrough: it owns the font map, text cursor, text colours and
the whole Adafruit-GFX-style glyph rasteriser (`_drawChar`/`_writeChar`/`_getTextBounds` in
`src/DrzGraphics.cpp`), and draws text by calling back into `instance->DrawPixel()`. So
`DrzEngine_PGE::DrawText` deliberately delegates to `DrzGraphics::DrawText` rather than to
`olc::PixelGameEngine::DrawString`. Text does nothing until the app has called `LoadFont`
+ `SetFont`.

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

An app implements `IDrzEngineApp` (`Setup` / `Update` / `Command` / `Data`). Rendering is
the app's job inside `Update`; there is no separate render callback at engine level.

`DisplayPageManager` is the optional UI layer above that: static list of `DisplayPage*`,
each owning `Widget*`s. The app calls `DisplayPageManager::ReadInputs/Update/Render` from
its `Update`, and pages navigate with `GoToPage(name)` / `Back()`. `Widget` draws in
widget-local coordinates and offsets by `screenX/screenY`.

## Layout

```
include/            public headers (all consumers add this to their include path)
  IDrz*.h           interfaces
  Drz*.h            static facades
  DrzEngine_PGE.h   PGE backend (also declares the private PixelGameEngineApp wrapper)
  FramebufferPlatform.h  olc::Platform + olc::Renderer over lib/fb (Linux framebuffer)
  DrzGraphics.h     Color/Sprite/font/rect/vec + IDrzGraphics + DrzGraphics
  gfx3d.h           GFX3D: the active 3D pipeline
  olcPixelGameEngine.h   vendored third party — do not edit or reformat
src/                implementations of the above
lib/fb/             vendored fbgraphics (C) — do not edit
examples/project_template/   skeleton for a downstream project
docs/Doxyfile.in    used when BUILD_DOC=ON
TestApp.{h,cpp}, test_drzengine_pge.cpp   manual smoke app at repo root
```

## The two 3D stacks

There are two, and they overlap. Know which one you are touching:

- **`gfx3d.h` / `src/gfx3d.cpp` — `drz::GFX3D`.** The active one, currently untracked in
  git (new work). A port of `olcPGEX_Graphics3D`: `GFX3D::Math` (`mat4x4`, `vec3d` with
  `w = 1`), `PipeLine` (projection / camera / transform / textures / 4 light slots /
  clipping), triangle rasterisers with a `float* m_DepthBuffer`. It renders through
  `IDrzGraphics` only, via the static `_gfx` set by `GFX3D::ConfigureDisplay(gfx)` — call
  that once, after the backend exists and before `ClearDepth()`/`Render()`.
- **`DrzGraphics.h` — `drz::graphics::Vector` / `Matrix4x4` / `Model`.** Older, thinner,
  row-vs-column conventions differ from `GFX3D::Math`, `Model` builds `triangleref`s that
  point into the caller's vertex vector. `TODO.md` still lists it as unfinished. Prefer
  `GFX3D` for new code; do not mix `graphics::vec3d` (`w` uninitialised by
  `Vector::Add`/`Sub`) with `GFX3D::vec3d` (`w = 1`) without converting explicitly.

## Conventions

- `#pragma once`, never include guards. `#pragma region` / `#pragma endregion` to group
  members — keep them balanced and named the same on both ends.
- Namespaces: `drz` for everything; `drz::graphics` and `drz::inputs` for the value types,
  re-exported into `drz` with `using namespace ...` at the bottom of the header.
- Public headers are included angle-style (`#include <DrzGraphics.h>`) because `include/`
  is on the include path of every target; `src/*.cpp` sometimes uses `"..."` for its own
  header. Both work — match the file you are editing.
- Doxygen `/** @brief ... */` on classes; plain `//` comments elsewhere. Comment density is
  low; do not add narration.
- Formatting is LLVM clang-format (2-space indent, 80 columns) — that is what the pending
  diff on `DrzGraphics.h` and `Widget.cpp` is. There is no `.clang-format` file, so
  `clang-format -i` uses LLVM defaults, which matches. **Only format files you actually
  changed**, and never `include/olcPixelGameEngine.h` or `lib/fb/**`.
- Interface changes are breaking changes for vanassistant (`IDrzSam::Setup()` just became
  `IDrzSam::Say(std::string)` in the working tree). When you touch an `IDrz*.h`, say so
  explicitly and check the consumer.

## Gotchas

- **`USE_FBGPLATFORM` leaks across targets.** `add_compile_definitions(USE_FBGPLATFORM)`
  sits inside the `BUILD_PGEFBLIB` block but is directory-scoped, so with both options ON
  (the default) `drzenginepge` is *also* compiled with the framebuffer platform — you get a
  library named "pge" that is really an FB build, silently. Always configure one backend
  per build directory. If you fix this, `target_compile_definitions(drzenginepgefb PRIVATE ...)`
  is the fix, and it must stay in sync with vanassistant's own
  `add_compile_definitions(USE_FBGPLATFORM)` for its FB platforms.
- **`OLC_PGE_APPLICATION` is defined exactly once**, in `src/DrzEngine_PGE.cpp`. Never
  define it elsewhere or the link breaks with duplicate symbols.
- **The custom platform macros must be set before `olcPixelGameEngine.h` is included**,
  which is why the `#undef OLC_PLATFORM_CUSTOM_EX` / `#define ... LinuxFBGPlatform` block is
  duplicated in both `DrzEngine_PGE.h` and `DrzEngine_PGE.cpp`. Keep the two copies identical.
- **`DrzEngine_PGE::DrawSprite` casts `drz::graphics::Sprite*` to `olc::Sprite*`**
  (`pge->DrawSprite(x, y, (olc::Sprite*)sprite, 1)`). It only works because the two classes
  happen to share a layout (`int32_t width, height`, then a `std::vector<Pixel|Color>`).
  Reordering or inserting members in `Sprite` — or adding a vtable to it — corrupts sprite
  drawing at runtime with no compiler error.
- **`olcPixelGameEngine.h` exists twice**: here and in `vanassistant/include/`. vanassistant's
  include path lists its own directory first, so *its* copy wins for its own translation
  units while this repo's copy is used for the library. If PGE behaviour differs between the
  two, that divergence is why.
- **`DisplayPageManager.h` has `using namespace drz;` at global scope in a header**, and
  `DisplayPage`/`DisplayPageManager` are themselves outside `namespace drz`. Anything
  including it inherits that. Don't copy the pattern; moving them into `drz` is a breaking
  change for vanassistant.
- **`TestApp.h` re-declares its own `IDrzEngineApp`** at global scope. It is dead code —
  inside `namespace drz`, lookup resolves to `drz::IDrzEngineApp` from the already-included
  `DrzEngine.h`, which is why it compiles. Delete the duplicate rather than following it.
- **`gfx3d.h` has a stray `}` inside `#ifdef EXPORT_TRIANGLES`** in `Model::_loadTris`
  (~line 157): the file only compiles while `EXPORT_TRIANGLES` is undefined. Fix the brace
  before using that debug path.
- `examples/project_template/CMakelists.txt` (lowercase `l`, and it is truncated — no
  targets) references a stale option name `BUILD_FBLIB`; the real one is `BUILD_PGEFBLIB`.
  Same stale name in vanassistant's CMakeLists, where it is harmless.
- `DrzEngine_PGE::Now()` reads `start`, which is only assigned in `Start()`; calling `Now()`
  before the main loop returns garbage.

## Consumer: vanassistant

`/home/drzoid/dev/vanassistant` does `add_subdirectory(/home/drzoid/dev/drzengine ...)`,
turns all four options OFF, then re-enables exactly one of `BUILD_PGELIB` / `BUILD_PGEFBLIB`
from its `-DPLATFORM=` (`LINUX_X11`, `PI_X11`, `WASM`, `WIN` -> PGE;
`LINUX_FB`, `PI_FB`, `BUILDROOT` -> PGEFB). It builds at C++17 while this library builds at
C++20 — keep the public headers C++17-clean.

Its `LINUX_FB` block does not link libpng itself, unlike `PI_FB` and `LINUX_X11`. That used
to fail at link with ~22 undefined `png_*` symbols; `drzenginepgefb` now declares the
dependency itself, so consumers get it transitively and the duplicate `find_package(PNG)`
calls in vanassistant's platform blocks are redundant (harmless, they can go whenever).

Before finishing an API change here, compile the consumer:

```bash
cmake -S /home/drzoid/dev/vanassistant -B <scratch>/va -DPLATFORM=LINUX_X11
cmake --build <scratch>/va -j$(nproc)
```

## Current working tree

`gfx3d.{h,cpp}` are new and untracked; `CMakeLists.txt` adds `src/gfx3d.cpp` to `SOURCES`;
`IDrzSam::Setup()` became `Say(std::string)`; `DrzGraphics.h` and `Widget.cpp` are
clang-format churn. Don't mistake the formatting diff for behaviour changes when reviewing.
