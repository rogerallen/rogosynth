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
- one note at a time, needs polyphony
- could use stereo pan
- needs a mixer.  Could use float buffer prior to mixer & have mixer output u16
- needs better ADSR envelope
- sine table only, could use square, triangle & sawtooth waves
- could use LPF after that

