#ifndef ROGOSYNTH_LOWPASSFILTER_H
#define ROGOSYNTH_LOWPASSFILTER_H
#include "constants.h"
extern "C" {
#include "sndfilter/biquad.h"
}
#include <assert.h>
#include <cstring>

class LowPassFilter {
    sf_biquad_state_st mState;
    float mCutoff;
    float mResonance;
    float mTempSamples[AUDIO_BUFFER_SAMPLES];

  public:
    LowPassFilter(float cutoff, float resonance)
        : mCutoff(cutoff), mResonance(resonance)
    {
        sf_lowpass(&mState, SAMPLE_RATE, cutoff, resonance);
    }

    void updateSamples(float *samples, long length)
    {
        assert(length == AUDIO_BUFFER_SAMPLES);
        sf_biquad_process(&mState, length / 2, (sf_sample_st *)samples,
                          (sf_sample_st *)mTempSamples);
        std::memcpy(samples, mTempSamples, sizeof(float) * length);
    }

    void cutoff(float v)
    {
        if (mCutoff != v) {
            mCutoff = v;
            sf_lowpass(&mState, SAMPLE_RATE, mCutoff, mResonance);
        }
    }
    float cutoff() { return mCutoff; }
    void resonance(float v)
    {
        if (mResonance != v) {
            mResonance = v;
            sf_lowpass(&mState, SAMPLE_RATE, mCutoff, mResonance);
        }
    }
    float resonance() { return mResonance; }
};
#endif