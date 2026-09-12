# Text rendering: `DrawText`, `GetTextBounds`, `GetFontAscent`

Everything about where a string ends up on screen when you call
`IDrzGraphics::DrawText(text, x, y, color)`. Read this before positioning text
by hand; if you find yourself adding a `+ 12` or a `- 2` to make a label land
where you want, the answer is almost always in here.

## The rule

**`(x, y)` is the top-left corner of the text's *line box*.** Not the baseline,
not the top of the first letter's ink. The line box is a band of fixed height
for a given font: it starts at `y`, and its baseline is `GetFontAscent()`
pixels lower.

```
 y  ──►  ┌───────────────────────────────────────┐  ▲
         │  (leading: only the tallest glyph,     │  │
         │   e.g. '(' or '|', reaches this high)  │  │ GetFontAscent()
         │  ██████  █████   ████  ██  ██          │  │
         │  ██  ██  ██  ██ ██     ██ ██           │  │
         │  ██████  █████  ██     ████            │  │
         │  ██  ██  ██  ██ ██     ██ ██   ██  ██  │  │
         │  ██████  ██  ██  ████  ██  ██   ████   │  │
baseline ├──────────────────────────────────▀▀────┤  ▼
         │  descenders (g, p, q, y) hang here  ██ │
         │                                    ███ │
         └───────────────────────────────────────┘
 x  ──►  ▲
         │ first glyph's ink starts at x + glyph.xOffset (usually x + 1)
```

Consequences you can rely on:

- `DrawText(s, 0, 0)` is entirely visible at the top of the screen: nothing
  is drawn above `y`. Horizontally, ink starts at `x + glyph.xOffset`, which
  is `+1` for the `Solid_Mono*` and `SevenSeg` fonts but can be negative in
  proportional ones (`Mono_Regular*` go down to `-7`), so `x = 0` may lose a
  column or two there.
- The vertical position depends only on the **font**, never on the string:
  `"5"`, `"g"`, `"BACK IN 5"` all share the same baseline for the same `y`.
  A value that changes every frame (a speed, a countdown) does not jump.
- Capitals and digits start a few pixels below `y` (2 px for
  `Solid_Mono8pt7b`, whose tallest glyphs reach 12 px above the baseline while
  capitals reach 10). That gap is the font's leading, and `GetTextBounds`
  reports it — see below.
- A `'\n'` in the string moves to the next line box, `font.yAdvance` pixels
  lower, and resets the pen to **screen column 0**, not to `x`. Multi-line
  text at `x > 0` is therefore a caller-side loop, one `DrawText` per line
  (see `VerticalTextAnimator` in vanassistant).
- Negative coordinates are legal: the text is clipped pixel by pixel by the
  backend's `DrawPixel`. Use them to scroll text off the top or the left.

## `GetTextBounds(text, x, y)`

Returns the rectangle of pixels that `DrawText(text, x, y, ...)` would paint
(the *ink* box), in screen coordinates, using the same `(x, y)` convention.
So with `b = GetTextBounds(s, x, y)`:

| Field | Meaning |
| --- | --- |
| `b.x` | left edge of the ink; `b.x - x` is the first glyph's `xOffset` (may be negative in proportional fonts) |
| `b.y` | top edge of the ink; `b.y - y` is the leading above the tallest character of *this string* (≥ 0) |
| `b.w`, `b.h` | ink size |

For an empty string, or without a font selected, the result is `{x, y, 0, 0}`.
(A space is not "no ink": Adafruit font tables give it a 1x1 placeholder
glyph on the baseline, so `"  "` reports a 1 px tall box — harmless, nothing
is painted for it.)

Because the bounds are relative to the pen position you passed, the usual
pattern is to compute them once at `(0, 0)` and reuse the offsets:

```cpp
rect b = gfx->GetTextBounds(text, 0, 0);
// centre the ink in a w x h box whose top-left is (bx, by)
gfx->DrawText(text, bx + (w - b.w) / 2 - b.x, by + (h - b.h) / 2 - b.y, WHITE);
// ink flush with the top of the box
gfx->DrawText(text, bx, by - b.y, WHITE);
// ink right-aligned to the box's right edge
gfx->DrawText(text, bx + w - (b.x + b.w), by, WHITE);
```

Ink-based centring moves with the string (`"ace"` sits lower than `"ACE"`).
When the text changes at runtime and must not move, position by the line box
instead: `DrawText(text, x, y)` with a fixed `y` and centre only horizontally.

## `GetFontAscent()`

Pixels from the top of the line box to the baseline for the current font, i.e.
the height of its tallest glyph above the baseline (`max(-glyph.yOffset)`,
scanned once per font and cached). `0` when no font is selected.

You need it whenever something must sit relative to the **baseline** rather
than to the ink: a text cursor whose bottom rests on the baseline is drawn at
`y + gfx->GetFontAscent() - cursorHeight`; an underline goes at
`y + gfx->GetFontAscent() + 1`.

Ascent and `yAdvance` (line spacing, from the font struct) of the fonts
shipped with vanassistant, for reference:

| Font | ascent | yAdvance | notes |
| --- | --- | --- | --- |
| `Solid_Mono4pt7b` | 5 | 11 | |
| `Solid_Mono6pt7b` | 9 | 16 | |
| `Solid_Mono8pt7b` | 12 | 22 | capitals/digits are 11 px tall, top at `y + 2` |
| `SevenSeg30pt7b` | 58 | 59 | `'1'` is drawn in the right part of its 42 px cell |

## Why not the baseline (Adafruit GFX)?

The rasteriser in `src/DrzGraphics.cpp` (`_drawChar`, `_writeChar`,
`_charBounds`, `_getTextBounds`) is a port of Adafruit GFX and its fonts are
Adafruit `GFXfont` tables: each glyph stores `yOffset`, the (negative)
distance from the **baseline** to its top row. In Adafruit GFX the cursor *is*
the baseline, so `setCursor(0, 0); print("A")` draws above the screen — and
every consumer ends up adding the glyph height by hand.

`DrzGraphics::DrawText` and `GetTextBounds` hide that: they shift the pen down
by `GetFontAscent()` before handing the string to the Adafruit code. The
private `_*` helpers still speak baseline; nothing outside `DrzGraphics.cpp`
should call them.

History: until 2026-09-12 the public `DrawText` *was* baseline-based, which is
why older code carried offsets like `y + bounds.h` or `y + 60`. When porting
such code, delete the offset rather than compensating for it.

## Checklist when text is not where you expect

1. Is a font selected? `DrawText` and `GetTextBounds` do nothing without
   `SetFont` (the PGE backend prints `[!] No font set`).
2. Are you adding the glyph height to `y`? Remove it — `y` is already the top.
3. Do you want the *ink* flush with an edge? Subtract `bounds.y` / `bounds.x`.
4. Does the text change at runtime? Position by the line box, not the ink.
5. Is `x` a widget-local coordinate? `Widget::DrawText` adds
   `screenX`/`screenY` for you; `gfx->DrawText` does not.
