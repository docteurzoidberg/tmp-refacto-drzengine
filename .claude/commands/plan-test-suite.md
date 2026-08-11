---
description: Draft the implementation plan for drzengine's test + benchmark suite (plan only, no code)
argument-hint: "[all|unit|render|perf|build]  (default: all)"
allowed-tools: Bash(cmake:*), Bash(nproc), Bash(git:*), Bash(ls:*), Bash(command:*), Bash(g++:*), Read, Glob, Grep
---

Produce an **implementation plan** for a complete test suite for this repository. Scope: `$1`
(default `all`).

The deliverable is the plan itself, written out for a human to approve. **Do not implement
anything** — no new source files, no CMake edits, no dependencies added. Read the code,
verify the claims below against it, then propose.

## Starting point

There is no test suite at all. The only runnable artefact is `test_drzengine_pge`, a manual
smoke app that needs X11, needs a pty at `/dev/pts/1` or it aborts, loops forever, is paced
by vsync, and animates from wall-clock `fElapsedTime` — so two builds of different speed
render different scenes. Nothing about it can be compared run to run.

Everything in `CHANGELOG.md` under *Performance* is marked "not measured on target
hardware". That is the gap this suite has to close, alongside plain correctness.

Re-check all of this yourself; this prompt may have drifted from the code.

## What "complete" has to mean here

Four axes. The plan covers those in `$1` (`all` = every axis), and says explicitly what it
leaves out.

**A. Unit — logic with no device.** The parts that need no screen: `GFX3D::Math` (matrix and
vector ops, `Mat_QuickInverse` vs `Mat_Inverse`, `Triangle_ClipAgainstPlane` in every case
including 0/1/2 output triangles), `DrzGraphics` text metrics and the glyph rasteriser
(`_getTextBounds` vs what `_drawChar` actually paints), `DisplayPageManager` navigation and
`Back()`, `Widget` local→screen offsetting, `Color`/`Sprite` semantics. Note where the two 3D
stacks disagree by design (`graphics::vec3d` leaves `w` uninitialised, `GFX3D::vec3d` sets
`w = 1`) — a test must not silently encode one convention as "the" one.

**B. Rendering non-regression — golden output.** Render fixed scenes into memory and compare
against a stored reference. Must report **how many pixels differ and by how much**, not just
pass/fail: legitimate float-level changes (the `1.0f/tex_w` reciprocal already in the tree)
shift a few texels without being a defect. Decide and justify: committed reference images, or
checksums only, or both at different granularity.

**C. Performance — deterministic benchmark.** Fixed scene, fixed frame count, constant `dt`
in the source, no vsync, no sleep, no wall-clock in the workload. Per-phase measurement (app
update / transform+clip / rasterise / text / present) — a single total cannot attribute a
gain. The primary metric must be reproducible on a noisy dev machine: instruction and
cache-miss counts under `valgrind --tool=callgrind|cachegrind` (deterministic) rather than
wall clock (±10-20 % under WSL2, which buries any gain smaller than that). Wall clock stays
as a sanity check, median of N runs.

For the target CPU: `arm-linux-gnueabihf-g++` and `qemu-arm-static` are installed, so the
plan can count instructions on real ARMv6 code (`-mcpu=arm1176jzf-s -mfpu=vfp`) — which is
where a stray libm call or a `fdivs` in an inner loop shows up and an x86 run hides it. State
the limit honestly in the plan: qemu counts instructions, not cycles; cachegrind's cache
model is a parameter, not the Pi; nothing here models SDRAM bandwidth or `FBIOPAN_DISPLAY`.
Compute-bound work is therefore verifiable without the board, presentation-bound work is not.

**D. Build and link matrix.** Cover the configurations that have already broken: both
`BUILD_PGELIB` and `BUILD_PGEFBLIB` in one configure (`USE_FBGPLATFORM` is directory-scoped
and silently makes `drzenginepge` a framebuffer build), `-DPLATFORM=LINUX_FB` on the consumer
(the libpng link), public headers staying C++17-clean for vanassistant, and `gfx3d.h` with
`EXPORT_TRIANGLES` defined (currently does not compile — stray brace).

## The problem to solve before any of it

A, B and C all need a way to run the engine with no X11, no `/dev/fb0` and no pty. Resolve
this first, in the plan, with a recommendation and a reason:

- a headless `IDrzEngine`/`IDrzGraphics` implementation in the repo, rendering into RAM;
- PGE kept, with a null platform/renderer pair alongside `LinuxFBGPlatform`;
- a mock local to the tests, engine untouched.

Note that `fbgraphics` exposes `fbg_customSetup()` (`lib/fb/include/fbgraphics.h`), so the
framebuffer present path — `DisplayFrame()`, the largest open performance item — can be
driven into a memory buffer without a real framebuffer. Whichever option you pick, say
whether it becomes part of the public API and therefore of the consumer's surface.

## Constraints the plan must respect

- No heavy test dependency. Either vendor one single-header framework or hand-roll a minimal
  runner — argue for one. Target rootfs is buildroot on a Pi Zero.
- Tests build with the **same flags the Pi gets** (Debug plus `-O2` forced by
  `DRZENGINE_OPTIMIZE`), or they measure a binary that ships nowhere.
- Must not lengthen the default build for the consumer. Pick the CMake option name and its
  default, and check it against how vanassistant turns options off then re-enables exactly
  one backend.
- Runs headless, offline, non-interactive, with a non-zero exit code on failure. CTest
  integration expected; say how `ctest` invokes each axis.
- Never edit or reformat `include/olcPixelGameEngine.h` or `lib/fb/**`. Never edit anything
  under `/home/drzoid/dev/vanassistant`.
- Build directories go to the scratchpad, not the user's `build/`.

## Shape of the plan

- **Phased**, each phase independently landable and useful on its own — no phase 4 that only
  pays off if 1-3 all land. First phase should be the smallest thing that makes a real defect
  visible.
- Per phase: files added, CMake changes, how it is run (exact commands), rough size, and one
  acceptance criterion that is checkable rather than aspirational.
- Directory layout for the tests, and where reference data lives.
- Tooling prerequisites with the install command for each, flagged as blocking or optional.
  `valgrind` and `perf` are absent today; `perf` is not usable under WSL2 anyway.
- **Sanity check the design**: for each defect already listed in `CHANGELOG.md` under *Known
  issues*, plus the two found earlier (the `(olc::Sprite*)` layout-punning cast, `Now()` read
  before `Start()` assigns `start`), state which phase would have caught it — or say plainly
  that none would. A suite that catches none of the bugs this repo actually has is the wrong
  suite.
- Risks and what the suite will still not tell us.

## Ask, do not guess

Put these to the user rather than inventing an answer:

- The representative workload. Triangle count per frame, textured or flat, resolution, how
  much text and how many widgets redraw — from the real vanassistant screens. A benchmark
  built on an invented scene optimises the wrong loop.
- Whether the suite is meant to run on the Pi too, or only on the dev machine.
- Whether binary reference images may be committed to git.
- Whether a test-only dependency is acceptable at all.

## Ground rules

Verify every factual claim against the code before building on it, and say which ones you
could not verify. Recommend, do not survey — one option per decision, with the reason.
Keep the plan short enough that someone can act on it in one sitting. Present it in the
conversation; only write it to a file if the user asks.

Implementation, when it is approved, follows the `CHANGELOG.md` rule in `CLAUDE.md`.
