# Changelog

Notable changes to DrzEngine. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

Nothing is tagged yet (`project(... VERSION 0.1.0)` in `CMakeLists.txt`, no git tags), so
everything lands under *Unreleased* until a first release is cut.

## [Unreleased]

### Added

- **`GFX3D` 3D pipeline** — `include/gfx3d.h`, `src/gfx3d.cpp`, a port of
  `olcPGEX_Graphics3D`: `GFX3D::Math` (`mat4x4`/`vec3d`), `PipeLine` (projection, camera,
  transform, texture, 4 light slots, near-plane and viewport clipping), and triangle
  rasterisers backed by a depth buffer. Renders only through `IDrzGraphics`, wired up by
  `GFX3D::ConfigureDisplay()`. Added to `SOURCES`, so both library targets build it.
  (`c579c16`)
- `CLAUDE.md`, plus the `/build` and `/check-consumer` project commands under
  `.claude/commands/`. (`4b4c1d7`)
- **A real `README.md`.** It was a one-line stub (`# tmp-refacto-drzengine`) untouched since
  the first commit. Now covers requirements, the per-backend build commands and the
  one-backend-per-build-directory rule, the CMake options, `add_subdirectory()` integration
  and the link requirements each backend does and does not propagate, a compiled-and-linked
  quick-start app, the facade/interface architecture, the frame lifecycle, and the vendored
  third-party licences.
- `DRZENGINE_OPTIMIZE` CMake option (default ON) — keeps the hot paths at `-O2` even when
  the build type is Debug or unset. Turn it off to step through the rasteriser. (`48ae37e`)

### Changed

- **Breaking:** `IDrzSam::Setup()` replaced by `IDrzSam::Say(std::string)`. (`c579c16`)
- `drzenginepgefb` now declares its own libpng dependency (`find_package(PNG REQUIRED)` +
  `target_link_libraries(... PRIVATE PNG::PNG)`). PGE auto-selects its libpng image loader
  on Linux and `src/DrzEngine_PGE.cpp` is the translation unit that compiles it, so the
  dependency belongs to this target. Consumers get it transitively. Side effect: a
  cross-compile sysroot without libpng now fails at configure time rather than at the
  consumer's link step. (`244cb86`)
- `LinuxFBGRenderer::PrepareDevice()` clears both pages of the flip chain once, at start-up,
  now that the per-frame clears are gone. (`48ae37e`)
- `include/DrzGraphics.h` and `src/Widget.cpp` reformatted with clang-format (LLVM style).
  No behaviour change. (`c579c16`)
- `.claude/settings.local.json` is gitignored; `.claude/commands/` stays versioned.
  (`4b4c1d7`)

### Fixed

- **Library was compiled at `-O0` in consumer builds.** A consumer using
  `add_subdirectory()` sets compile options on its own executable only and picks the build
  type for everyone; vanassistant defaults to Debug, so the rasteriser, text rendering and
  widgets were all unoptimised on the Pi. Optimisation flags are now applied per target.
  (`48ae37e`)
- **vanassistant `-DPLATFORM=LINUX_FB` failed to link** with ~22 undefined `png_*` symbols.
  Fixed by the libpng dependency declaration above, with no change to vanassistant.
  (`244cb86`)
- **The emscripten build did not compile.** `DrzEngine_PGE.cpp` included `<DrzSerial_Null.h>`
  under `__EMSCRIPTEN__` but that header had never been written, and the matching branch of
  the constructor created no serial backend at all, leaving `DrzSerial::Get()` null for every
  caller. Added a no-op `IDrzSerial` implementation for platforms with no serial port (reads
  report no byte available, writes are discarded) and instantiated it there. (`6e8949b`)

### Performance

Not measured on target hardware — these remove work rather than trade accuracy for speed.

- `LinuxFBGRenderer::PrepareDrawing()`, `ClearBuffer()` and `DrawLayerQuad()` are no-ops.
  PGE calls all three every frame and each wiped the whole screen, while `DisplayFrame()`
  rewrites every pixel it owns immediately afterwards. (`48ae37e`)
- `GFX3D` caches the screen size in `ConfigureDisplay()` instead of calling the virtual
  `GetScreenWidth()` once per pixel (twice when the depth test passes) to index the depth
  buffer — 8 call sites. (`48ae37e`)
- `GFX3D` texture sampling computes one reciprocal instead of two divisions per pixel, and
  only when the depth test passes, so occluded pixels no longer pay for it. Not bit-identical
  in floating point (one extra rounding); irrelevant here, the result indexes integer texels.
  (`48ae37e`)

### Known issues

- Enabling `BUILD_PGELIB` and `BUILD_PGEFBLIB` in a single configure silently builds
  `drzenginepge` as a framebuffer library: `add_compile_definitions(USE_FBGPLATFORM)` is
  directory-scoped. Configure one backend per build directory. See `CLAUDE.md`.
- `include/gfx3d.h` does not compile with `EXPORT_TRIANGLES` defined — stray brace in
  `Model::_loadTris`.
- `PipeLine::Render` allocates per frame, and a `std::list` per triangle, in the clipping
  path.
- `LinuxFBGRenderer::DisplayFrame()` copies the frame one pixel at a time through a
  non-inlinable call, with a pointless (and slightly darkening) alpha blend.
