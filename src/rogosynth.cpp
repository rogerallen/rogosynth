#include "rogosynth.h"
#include "audio.h"

#ifdef MTR_ENABLED
#include "minitrace/minitrace.h"
#else
#define mtr_init(X) {}
#define mtr_shutdown() {}
#define MTR_BEGIN(X,Y) {}
#define MTR_END(X,Y) {}
#endif

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

    MTR_BEGIN("RogoSynth", "voices");
    // add all active synths together
    int numActiveSynths = 0;
    for (int i = 0; i < NUM_SYNTHS; i++) {
        if (mSynths[i]->active()) {
            numActiveSynths++;
        }
        // always call addSamples so time & phase are consistent
        mSynths[i]->addSamples(samples, AUDIO_BUFFER_SAMPLES);
    }
    MTR_COUNTER("RogoSynth", "numVoices", numActiveSynths);
    MTR_END("RogoSynth", "voices");
    MTR_BEGIN("RogoSynth", "pan");
    // pan synths left/right
    pan(samples, AUDIO_BUFFER_SAMPLES, mPanPosition);
    MTR_END("RogoSynth", "pan");
    // compressor to try to keep synths from cracking
    MTR_BEGIN("RogoSynth", "compressor");
    mCompressor->updateSamples(samples, AUDIO_BUFFER_SAMPLES);
    MTR_END("RogoSynth", "compressor");
    // low pass resonant filter
    MTR_BEGIN("RogoSynth", "LPF");
    mLowPassFilter->updateSamples(samples, AUDIO_BUFFER_SAMPLES);
    MTR_END("RogoSynth", "LPF");
    // reverb
    MTR_BEGIN("RogoSynth", "reverb");
    mReverb->updateSamples(samples, AUDIO_BUFFER_SAMPLES);
    MTR_END("RogoSynth", "reverb");

}
