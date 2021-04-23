# rogosynth

SDL2 audio synthesis.

Initially based on https://github.com/lundstroem/synth-samples-sdl2 example number 3.  Started with this simple code and evolved it into a polyphonic synth.

I wanted more features and grabbed some audio code for filtering, compression, etc. from https://github.com/velipso/sndfilter (Thanks, Sean!)

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
- [DONE] needs a compressor
- [DONE] could use resonant LPF after synth
- low freq osc input to offset pitch, amplitude, phase
- adjust pitch indexes to match midi 
