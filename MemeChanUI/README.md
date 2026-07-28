# MemeChan Screaming!! DELUXE — UI mock-up

A first pass at the editor layout, built with native JUCE drawing so it can be
dropped straight into the plugin's `AudioProcessorEditor` later.

## Build & render

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
./build/MemeChanUIRender_artefacts/Release/MemeChanUIRender . 1.0
```

`MemeChanUIRender` paints every tab into a PNG (`preview-amp.png`,
`preview-drive.png`, `preview-fx.png`, `preview-routing.png`) without opening a
window, so layout changes can be checked in a second or two instead of loading a
host. On a headless box run it under `xvfb-run -a`. The first argument is the
output directory, the second a render scale — pass `2.0` for a HiDPI check.

## Files

| File | Contents |
|------|----------|
| `Source/Theme.*` | Palette, typography, card/shadow helpers, placeholder icon paths |
| `Source/Widgets.*` | `PawKnob`, `LevelMeter`, `FlatButton`, `SegmentedControl`, `TabStrip`, `FxChainStrip`, `WaveformView`, and the shared `LookAndFeel` |
| `Source/HeroBanner.*` | The illustrated header, drawn procedurally |
| `Source/Pages.*` | `PagePanel` plus the four tabs, described with `add*()` calls on a shared grid |
| `Source/MemeChanEditor.*` | Frame layout — design size 1448 × 1086, scaled from a single factor |
| `Source/RenderMain.cpp` | Offline PNG renderer |

Pages lay themselves out in design units and the editor applies the window scale
via `setTransform`, so page code never deals with scaling. Adding a tab is a
`create*Page()` function plus an entry in the `TabStrip`.

## Skins

`theme::setSkin()` swaps the whole palette. Widgets read `colours::*`,
`theme::strokeWidth()` and `theme::strokeInked()` rather than branching on the
skin themselves, so one call re-skins every page.

The comic skin builds its look from `roughRoundedRect()` (hand-drawn wobble),
`strokeInked()` (dry, broken outlines), `halftone()` (screentone) and
`paperGrain()` (tiled stock). Erosion strength is one number —
`theme::setInkBreakup()`, 0 for solid, ~0.5 for heavily broken.

`strokeInked()` rebuilds its geometry on every call. That is fine for static
chrome but should not run per frame, so anything animating (meters, waveforms)
belongs in its own component, and panels that only redraw on resize should set
`setBufferedToImage (true)`.

## What is a placeholder

* **All artwork.** The cat, the comic burst, the speed lines and the knob faces
  are drawn in code. In the shipping build they become `BinaryData` images —
  in particular the knobs should be film-strips rendered from the real
  photograph rather than anything drawn at runtime.
* **Fonts.** The mock-up uses an Inter-like grotesque and a Japanese brush face;
  this render only has Liberation Sans and IPAGothic available. Embedding the
  real faces needs a licence that permits it.
* **Values.** Every control holds a static value. Nothing is connected to a
  processor yet — each one needs its `AudioProcessorValueTreeState` attachment.
* **Controls on the DRIVE / FX / ROUTING tabs.** The AMP tab follows the
  approved mock-up. The other three are a plausible guess at a NAM + IR signal
  path so the tabs are not empty — treat the parameter names as a starting point
  to argue with, not a spec.
