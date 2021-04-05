# rogosynth

SDL2 audio synthesis.

Initially based on https://github.com/lundstroem/synth-samples-sdl2 example number 3.

Just a simple implementation, wanted to see how it could work.

Issues:
- getting compiling on Windows is having issues.  So annoying...
- one note at a time, needs polyphony
- could use stereo pan
- needs a mixer.  Could use float buffer prior to mixer & have mixer output u16
- needs better ADSR envelope
- sine table only, could use square, triangle & sawtooth waves
- could use LPF after that

