---
description: Configure and build a drzengine backend out of tree (pge | fb | test | all)
argument-hint: "[pge|fb|test|all]  (default: pge)"
allowed-tools: Bash(cmake:*), Bash(nproc), Read, Glob, Grep
---

Build drzengine. Target: `$1` (default `pge` if empty).

Build **into the scratchpad**, never into the user's `build/` — that directory holds the
`compile_commands.json` clangd is configured to use. Use one build directory per backend;
never enable both libraries in a single configure (`USE_FBGPLATFORM` is directory-scoped and
would silently turn `drzenginepge` into a framebuffer build).

| `$1` | configure |
| --- | --- |
| `pge` | `-DBUILD_DOC=OFF -DBUILD_TEST_APPS=OFF -DBUILD_PGEFBLIB=OFF` |
| `fb` | `-DBUILD_DOC=OFF -DBUILD_TEST_APPS=OFF -DBUILD_PGELIB=OFF` |
| `test` | `-DBUILD_DOC=OFF -DBUILD_PGEFBLIB=OFF` (adds `test_drzengine_pge`, needs PNG/OpenGL/X11) |
| `all` | run `pge` and `fb` above in two separate build directories |

Then:

```
cmake -S /home/drzoid/dev/drzengine -B <scratch>/<name> <options>
cmake --build <scratch>/<name> -j$(nproc)
```

Report compiler warnings that point at code touched in this session; ignore pre-existing
warnings from `include/olcPixelGameEngine.h` and `lib/fb/**` (vendored).

If the build fails, fix the cause in the source — do not work around it by disabling a
target or loosening a flag.
