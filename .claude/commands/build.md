---
description: Configure and build a drzengine backend out of tree (pge | fb | test | test-fb | all)
argument-hint: "[pge|fb|test|test-fb|all]  (default: pge)"
allowed-tools: Bash(cmake:*), Bash(nproc), Read, Glob, Grep
---

Build drzengine. Target: `$1` (default `pge` if empty).

Build **into the scratchpad**, never into the user's `build/` — that directory holds the
`compile_commands.json` clangd is configured to use. Use one build directory per backend;
never enable both libraries in a single configure (`USE_FBGPLATFORM` is directory-scoped and
would silently turn `drzenginepge` into a framebuffer build).

| `$1` | configure | builds |
| --- | --- | --- |
| `pge` | `-DBUILD_DOC=OFF -DBUILD_TEST_APPS=OFF -DBUILD_PGEFBLIB=OFF` | `libdrzenginepge.a` |
| `fb` | `-DBUILD_DOC=OFF -DBUILD_TEST_APPS=OFF -DBUILD_PGELIB=OFF` | `libdrzenginepgefb.a` |
| `test` | `-DBUILD_DOC=OFF -DBUILD_PGEFBLIB=OFF` | `libdrzenginepge.a` + `test_drzengine_pge` (needs PNG/OpenGL/X11/pthread) |
| `test-fb` | `-DBUILD_DOC=OFF -DBUILD_PGELIB=OFF` | `libdrzenginepgefb.a` + `test_drzengine_pgefb` (needs PNG/pthread, no X11) |
| `all` | run `test` and `test-fb` above in two separate build directories | both libs + both test apps |

`BUILD_TEST_APPS` gates both executables but each one also needs its own backend
(`test_drzengine_pge` under `BUILD_PGELIB`, `test_drzengine_pgefb` under `BUILD_PGEFBLIB`),
so a test build always produces the matching library too — `all` therefore covers everything
in two configures, and does not need a separate `pge`/`fb` pass.

Then:

```
cmake -S /home/drzoid/dev/drzengine -B <scratch>/<name> <options>
cmake --build <scratch>/<name> -j$(nproc)
```

Compile and link only. Do not run the test apps: `test_drzengine_pge` opens an X11 window,
`test_drzengine_pgefb` writes to the real framebuffer, and both call `serial->Setup()` on
`/dev/pts/1`, which fails unless that pty exists.

Report compiler warnings that point at code touched in this session; ignore pre-existing
warnings from `include/olcPixelGameEngine.h` and `lib/fb/**` (vendored).

If the build fails, fix the cause in the source — do not work around it by disabling a
target or loosening a flag.
