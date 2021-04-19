#ifndef ROGOSYNTH_SYNTH_H
#define ROGOSYNTH_SYNTH_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>

#include "envelope.h"

const int SAMPLE_RATE = 44100;
const double CHROMATIC_BASE = pow(2.0, 1.0/12.0); // 2^(1/12)=1.059463094359295264562;
const int MIN_NOTE = 12;
const int MAX_NOTE = 131;
const int TABLE_LENGTH = 1024;
    
class Synth {
    static int16_t *cSineWaveTable;
    
    double mCurPhase;
    double mCurTime;

    int mNote;
    int mOctave;
    bool mKeyPressed;

    Envelope mEnvelope;

  public:
    Synth();
    //~Synth();
    void noteOn(int note);
    void noteOff();
    void writeSamples(int16_t *samples, long length);
    void attack(double v) { mEnvelope.attack(v); }
    double attack() { return mEnvelope.attack(); }
    void decay(double v) { mEnvelope.decay(v); }
    double decay() { return mEnvelope.decay(); }
    void sustain(double v) { mEnvelope.sustain(v); }
    double sustain() { return mEnvelope.sustain(); }
    void release(double v) { mEnvelope.release(v); }
    double release() { return mEnvelope.release(); }
    bool active() { return mEnvelope.active(mCurTime); }
    bool releasing() { return mEnvelope.releasing(mCurTime); }
};
#endif