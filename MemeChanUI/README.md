# MemeChan Screaming!! DELUXE — UI mock-up

A first pass at the editor layout, built with native JUCE drawing so it can be
dropped straight into the plugin's `AudioProcessorEditor` later.

## Build & render

```
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
./build/MemeChanUIRender_artefacts/Release/MemeChanUIRender preview.png 1.0
```

`MemeChanUIRender` paints the editor into a PNG without opening a window, so
layout changes can be checked in a second or two instead of loading a host. On a
headless box run it under `xvfb-run -a`. The second argument is a render scale —
pass `2.0` for a HiDPI check.

## Files

| File | Contents |
|------|----------|
| `Source/Theme.*` | Palette, typography, card/shadow helpers, placeholder icon paths |
| `Source/Widgets.*` | `PawKnob`, `LevelMeter`, `FlatButton`, `SegmentedControl`, `TabStrip`, `FxChainStrip`, `WaveformView`, and the shared `LookAndFeel` |
| `Source/HeroBanner.*` | The illustrated header, drawn procedurally |
| `Source/MemeChanEditor.*` | Full layout — design size 1448 × 1086, scaled from a single factor |
| `Source/RenderMain.cpp` | Offline PNG renderer |

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
* **Pages.** Only the AMP tab is laid out. DRIVE / FX / ROUTING are empty.
