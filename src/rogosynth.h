#ifndef ROGOSYNTH_H
#define ROGOSYNTH_H
#include "compressor.h"
#include "constants.h"
#include "lowpassfilter.h"
#include "reverb.h"
#include "synthvoice.h"

class RogoSynth {

    static const int NUM_SYNTHS = 8;
    const float SYNTH_AMPLITUDE = 1.0f / NUM_SYNTHS;

    SynthVoice *mSynths[NUM_SYNTHS];
    float mPanPosition;
    // These need to be on the heap, not the stack as the state structures
    // are pretty big.
    Compressor *mCompressor;
    LowPassFilter *mLowPassFilter;
    Reverb *mReverb;

  public:
    RogoSynth();
    ~RogoSynth();
    void updateSamples(float *samples, long length);
    // getters/setters
    int numSynths() { return NUM_SYNTHS; }
    bool active(int voice) { return mSynths[voice]->active(); }
    void noteOn(int voice, int pitch) { mSynths[voice]->noteOn(pitch); }
    void noteOff(int voice) { mSynths[voice]->noteOff(); }
    int pitch(int voice) { return mSynths[voice]->pitch(); }
    bool releasing(int voice) { return mSynths[voice]->releasing(); }
    float amplitude() { return mSynths[0]->amplitude(); }
    void amplitude(float v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->amplitude(v);
        }
    }
    float attack() { return mSynths[0]->attack(); }
    void attack(float v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->attack(v);
        }
    }
    float decay() { return mSynths[0]->decay(); }
    void decay(float v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->decay(v);
        }
    }
    float sustain() { return mSynths[0]->sustain(); }
    void sustain(float v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->sustain(v);
        }
    }
    float release() { return mSynths[0]->release(); }
    void release(float v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->release(v);
        }
    }
    WaveType type() { return mSynths[0]->type(); }
    void type(WaveType v)
    {
        for (int i = 0; i < NUM_SYNTHS; i++) {
            mSynths[i]->type(v);
        }
    }
    float panPosition() { return mPanPosition; }
    void panPosition(float v) { mPanPosition = v; }
    float lpfCutoff() { return mLowPassFilter->cutoff(); }
    void lpfCutoff(float v) { mLowPassFilter->cutoff(v); }
    float lpfResonance() { return mLowPassFilter->resonance(); }
    void lpfResonance(float v) { mLowPassFilter->resonance(v); }
    sf_reverb_preset reverbPreset() { return mReverb->preset(); }
    void reverbPreset(sf_reverb_preset v) { mReverb->preset(v); }
};
#endif