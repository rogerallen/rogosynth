#ifndef ROGOSYNTH_REVERB_H
#define ROGOSYNTH_REVERB_H
#include "constants.h"
extern "C" {
#include "sndfilter/reverb.h"
}
#include <assert.h>
#include <cstring>
#include <iostream>

class Reverb {
    sf_reverb_state_st mState;
    sf_reverb_preset mPreset;
    float mTempSamples[AUDIO_BUFFER_SAMPLES];

public:
    // NOTE: For some reason allocating this on
    // the stack results in corruption.  Allocate
    // on the heap via new instead.
    Reverb(sf_reverb_preset preset) : mPreset(preset) 
    {
        sf_presetreverb(&mState, SAMPLE_RATE, preset);
    }
        
    void updateSamples(float *samples, long length)
    {
        assert(length == AUDIO_BUFFER_SAMPLES);
        sf_reverb_process(&mState, length / 2, (sf_sample_st *)samples,
                          (sf_sample_st *)mTempSamples);
        std::memcpy(samples, mTempSamples, sizeof(float) * length);
    }

    void preset(sf_reverb_preset v)
    {
        if (mPreset != v) {
            mPreset = v;
            sf_presetreverb(&mState, SAMPLE_RATE, mPreset);
        }
    }
    sf_reverb_preset preset() { return mPreset; }
};
#endif
