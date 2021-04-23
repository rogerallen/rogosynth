#ifndef ROGOSYNTH_CONSTANTS_H
#define ROGOSYNTH_CONSTANTS_H
const int AUDIO_BUFFER_STEREO_SAMPLES = 4096; // must be power of two.
const int AUDIO_BUFFER_SAMPLES = AUDIO_BUFFER_STEREO_SAMPLES * 2;
const int SAMPLE_RATE = 44100;
const float TIME_INC = 1.0f / SAMPLE_RATE;
const float CHROMATIC_BASE = powf(2.0f, 1.0f / 12.0f); // 2^(1/12)
#endif