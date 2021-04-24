#ifndef ROGOSYNTH_CONSTANTS_H
#define ROGOSYNTH_CONSTANTS_H
#define _USE_MATH_DEFINES
#include <cmath>
const int AUDIO_BUFFER_STEREO_SAMPLES = 2048; // must be power of two.
const int AUDIO_BUFFER_SAMPLES = AUDIO_BUFFER_STEREO_SAMPLES * 2;
const int SAMPLE_RATE = 44100;
const float SAMPLE_PERIOD = 1.0f / SAMPLE_RATE;
const float CHROMATIC_BASE = powf(2.0f, 1.0f / 12.0f);
#endif