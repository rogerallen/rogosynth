#include "rogosynth.h"
#include "audio.h"

RogoSynth::RogoSynth() 
{
    for (int i = 0; i < NUM_SYNTHS; i++) {
        mSynths[i] = new SynthVoice(SYNTH_AMPLITUDE);
    }
    mPanPosition = 0.0f;
    mCompressor = new Compressor();
    mLowPassFilter = new LowPassFilter(500.0f, 5.0f);
    mReverb = new Reverb(SF_REVERB_PRESET_DEFAULT);

}

RogoSynth::~RogoSynth()
{
    for (int i = 0; i < NUM_SYNTHS; i++) {
        delete mSynths[i];
    }
    delete mCompressor;
    delete mLowPassFilter;
    delete mReverb;
}

void RogoSynth::updateSamples(float *samples, long length)
{
    // add all active synths together
    int numActiveSynths = 0;
    for (int i = 0; i < NUM_SYNTHS; i++) {
        if (mSynths[i]->active()) {
            numActiveSynths++;
        }
        // always call addSamples so time & phase are consistent
        mSynths[i]->addSamples(samples, AUDIO_BUFFER_SAMPLES);
    }
    // pan synths left/right
    pan(samples, AUDIO_BUFFER_SAMPLES, mPanPosition);
    // compressor to try to keep synths from cracking
    mCompressor->updateSamples(samples, AUDIO_BUFFER_SAMPLES);
    // low pass resonant filter
    mLowPassFilter->updateSamples(samples, AUDIO_BUFFER_SAMPLES);
    // reverb
    mReverb->updateSamples(samples, AUDIO_BUFFER_SAMPLES);

}
