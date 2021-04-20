#ifndef ROGOSYNTH_SYNTH_H
#define ROGOSYNTH_SYNTH_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

#include "envelope.h"

const int SAMPLE_RATE = 44100;
const float TIME_INC = 1.0f / SAMPLE_RATE;
const float CHROMATIC_BASE = powf(2.0f, 1.0f / 12.0f); // 2^(1/12)
const int MIN_NOTE = 12;
const int MAX_NOTE = 131;
const int TABLE_LENGTH = 1024;

class Synth {
    static int16_t *cSineWaveTable;
    float mCurPhase;
    float mCurTime;
    int mPitch;
    Envelope mEnvelope;

  public:
    Synth();
    void noteOn(int pitch);
    void noteOff();
    int pitch() { return mPitch; }
    void addSamples(float *samples, long length);
    void attack(float v) { mEnvelope.attack(v); }
    float attack() { return mEnvelope.attack(); }
    void decay(float v) { mEnvelope.decay(v); }
    float decay() { return mEnvelope.decay(); }
    void sustain(float v) { mEnvelope.sustain(v); }
    float sustain() { return mEnvelope.sustain(); }
    void release(float v) { mEnvelope.release(v); }
    float release() { return mEnvelope.release(); }
    bool active() { return mEnvelope.active(mCurTime); }
    bool releasing() { return mEnvelope.releasing(mCurTime); }
};
#endif