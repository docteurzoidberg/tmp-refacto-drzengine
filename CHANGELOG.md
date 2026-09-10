# Changelog

Notable changes to DrzEngine. Format loosely follows
[Keep a Changelog](https://keepachangelog.com/en/1.1.0/).

Nothing is tagged yet (`project(... VERSION 0.1.0)` in `CMakeLists.txt`, no git tags), so
everything lands under *Unreleased* until a first release is cut.

## [Unreleased]

### Added

- **`IDrzGraphics::GetPixelBuffer()`** — optional direct access to a backend's flat
  32-bit pixel buffer, so `GFX3D`'s rasteriser can fill a flat span without a virtual
  `DrawPixel` call and a redundant bounds check per pixel. Not pure virtual: the default
  returns `nullptr` and every existing backend keeps the per-pixel path unchanged, so
  nothing changes until a backend opts in.

  Measured with `gfx3d-bench --direct` (J7, 320x240, four repeats): frame mean 0.227 ms
  -> 0.211 ms, a further 7% on top of the 28.6% below, image unchanged. A small part of
  that is the benchmark's own per-pixel counter disappearing along the direct path.

  **No backend implements it yet**, and one should only do so if bypassing `DrawPixel`
  is acceptable for it — the direct write skips `SetPaintMode` handling, which is fine
  for opaque 3D but not in general, and it assumes the buffer is `Color`-sized and
  `GetScreenWidth()` pixels per row.

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

- **`GFX3D::ConfigureDisplay()` leaked a full depth buffer per call**
  (`src/gfx3d.cpp`). It allocated `new float[w * h]` every time and dropped the
  previous buffer on the floor, so each additional caller leaked a screen's
  worth of floats. It now reuses the buffer when the screen size is unchanged
  and frees the old one when it is not, which makes it safe for several scene
  widgets to configure the display independently.

- **`GFX3D::PipeLine` rendered with uninitialised light slots** (`include/gfx3d.h`). The
  `lights[4]` member array had no initialiser and `PipeLine::PipeLine()` never touched it,
  so every slot the caller did not explicitly set carried an indeterminate `type`.
  `Render()` switches on that field, so a garbage slot could register itself as an ambient
  or directional light with a garbage colour and direction, and perturb the shading of
  arbitrary faces differently on every run. The array is now value-initialised, which puts
  all four slots at `LIGHT_DISABLED`.

- **`GFX3D` lighting wrapped `uint8_t` on over-lit surfaces** (`src/gfx3d.cpp`). Each
  directional light adds up to `1.0` to `nLightR/G/B`, and the accumulator was only floored
  against the ambient term, never capped. With two or more directional lights the product
  `nLight * col` overflowed 255 and wrapped back down (`1.5 * 200` → `44`), so the brightest
  surfaces rendered *darker* than dimly lit ones. The accumulator is now clamped to `1.0`.
- **`GFX3D` depth buffer was read out of bounds** (`src/gfx3d.cpp`, 4 rasteriser sites). The
  depth *write* was guarded by `DrawPixel()`'s return value, but the *read*
  `m_DepthBuffer[i * _screenW + j]` was not. After the clipped coordinates are truncated to
  int the rasteriser can step one pixel past the viewport, so at the right edge it compared
  against the next row's depth and on the last row it read past the end of the allocation.
  Both are now routed through a bounds-checked `DepthIndex()` helper.
- Dropped a dead `if (light > 0) { int j = 0; }` left over from debugging the directional
  light path.

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

**`GFX3D::PipeLine` frame time cut by 28.6%** on the J7 body (1456 triangles,
320x240, flat + depth + lights + cull_cw): mean 0.318 ms -> 0.227 ms, 3148 ->
4400 FPS. Measured with `gfx3d-bench` over 1000 frames, four repeats, spread
under 2%; x86-64 `-O2`, not yet measured on the Pi. Every change below was
checked with `gfx3d-bench --verify` against a recording of the pre-change
renderer, across six configurations (J7, Scout, 3dface; wire, cull_ccw, no
depth, 640x480). All are faster by 14-32% and none moved a pixel by more than
one least-significant bit.

Splitting the frame by rendering at several resolutions puts the per-triangle
stage at 0.159 ms and the rasteriser at ~13.7 ns/pixel before, 0.097 ms and
~12.2 ns/pixel after: the geometry stage lost 39%, the rasteriser 11%.

- **The clipper no longer normalises what is already normal.**
  `Triangle_ClipAgainstPlane()` normalised its plane normal on entry and
  `Vec_IntersectPlane()` did it again on each of its calls, though every call
  site clips against axis-aligned unit vectors. Its distance lambda also
  opened by normalising the vertex into a variable it never read, and
  recomputed the plane's constant term once per vertex. Five planes per
  triangle made this the single largest waste in the pipeline.
- **Light directions are normalised once in `SetLightSource()`**, not once per
  triangle per light in `Render()`. The direction does not change between
  triangles.
- **Backface culling tests the raw cross product.** Normalising divides by a
  positive length and cannot change the sign of the dot product, so the
  normalise is now done only for the triangles that survive and need shading
  — roughly half of them on a closed mesh.
- **Viewport clipping no longer allocates.** It ran through a `std::list` built
  per triangle: a heap allocation and free for every triangle that survived
  culling, several hundred malloc/free pairs per frame, to hold at most a
  handful of triangles. Two fixed stack buffers hold the same triangles in the
  same order.
- **Triangles wholly inside the viewport skip clipping entirely.** They come
  out of all four plane clips unchanged, which is the normal case for a model
  that fits on screen.
- **Flat spans interpolate no colour.** All three vertices of a flat-shaded
  triangle carry the same colour, so the rasteriser spent four multiply-adds
  and four float-to-byte conversions per pixel arriving back where it started
  — and not even reliably: `(1-t)*c + t*c` can land just under `c` and
  truncate to `c-1`. This is the source of the 1-LSB differences above, and
  the new value is the correct one. The compiler cannot do this: it has no way
  to know the three colours are equal.
- **The rasteriser tests depth before shading.** Occluded pixels no longer pay
  for a colour that is then thrown away.
- **The two halves of `RasterTriangle()` share one span routine.** They ran
  textually identical copies of the inner loop. On x86 `-O2` this was worth
  nothing on its own — the compiler already hoisted the flag tests and dropped
  the unread `u`/`v` interpolation — but it is what makes the flat-colour path
  above expressible once instead of twice.
- Per-triangle setup no longer divides out `u`/`v` steps with no texture bound,
  or colour steps on a flat triangle. Bit-exact, and worth nothing measurable
  on x86 where the divider is cheap and the compiler had already eliminated
  most of it; kept for ARM, where VFP division is slow and unpipelined. Unmeasured
  on target.

Earlier work, not measured on target hardware — these remove work rather than
trade accuracy for speed.

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
