#ifndef ROGOSYNTH_COMPRESSOR_H
#define ROGOSYNTH_COMPRESSOR_H
#include "constants.h"
extern "C" {
#include "sndfilter/compressor.h"
}
#include <cstring>
class Compressor {
    sf_compressor_state_st mState;
    float mTempSamples[AUDIO_BUFFER_SAMPLES];
    // TODO add compressor settings, getters, setters
  public:
    Compressor() { sf_defaultcomp(&mState, SAMPLE_RATE); }

    void updateSamples(float *samples, long length)
    {
        assert(length == AUDIO_BUFFER_SAMPLES);
        sf_compressor_process(&mState, length / 2, (sf_sample_st *)samples,
                              (sf_sample_st *)mTempSamples);
        std::memcpy(samples, mTempSamples, sizeof(float) * length);
    }
};
#endif