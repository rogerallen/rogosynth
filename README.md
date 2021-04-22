# rogosynth

SDL2 audio synthesis.

Initially based on https://github.com/lundstroem/synth-samples-sdl2 example number 3.

Just a simple implementation, wanted to see how it could work.

## Building

Linux Ubuntu 20.04 builds with apt installed SDL & GLEW

Got Windows Build working with both:
1) VS Code using Microsoft CMake & Select "VS Community 2019 Release - amd64" kit
   output in "build"
2) VS 2019 Community Edition
   output in "out/build"

## Issues
- [DONE] needs better ADSR envelope
- [DONE] one note at a time, needs polyphony
- [DONE] basic GUI
- [DONE] sine, square, triangle & sawtooth waves
- [DONE] could use stereo pan
- needs a compressor
- could use resonant LPF after synth
- low freq osc input to offset pitch, amplitude, phase
