---
description: Verify drzengine changes still compile against the vanassistant consumer
allowed-tools: Bash(cmake:*), Bash(nproc), Bash(git:*), Read, Glob, Grep
---

`/home/drzoid/dev/vanassistant` consumes this library through `add_subdirectory`, so every
change to a public header in `include/` is a potential break there. Run this after touching
any `IDrz*.h`, `Drz*.h`, `Widget.h`, `DisplayPageManager.h` or `gfx3d.h`.

1. List the public headers changed in the working tree (`git status --short include/`) and
   the API elements that actually changed signature or moved namespace.
2. Grep vanassistant (`src/`, `include/`) for each of those symbols to see who is affected.
3. Build the consumer for the X11 platform, into the scratchpad:

   ```
   cmake -S /home/drzoid/dev/vanassistant -B <scratch>/va-x11 -DPLATFORM=LINUX_X11
   cmake --build <scratch>/va-x11 -j$(nproc)
   ```

   For a framebuffer-facing change, use `-DPLATFORM=LINUX_FB` in a *separate* directory.

4. Report breaks as: symbol → call sites in vanassistant → the minimal fix on each side.
   Note that vanassistant builds at **C++17** while this library builds at C++20, so a C++20
   construct in a public header breaks it even when drzengine itself compiles.

Do not edit files under `/home/drzoid/dev/vanassistant` unless the user asks — it is a
separate repository. Report what needs changing there and let them decide.
