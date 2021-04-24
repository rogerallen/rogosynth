#ifndef ROGOSYNTH_SYNTH_H
#define ROGOSYNTH_SYNTH_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdint>
#include "constants.h"
#include "envelope.h"

const int MIN_NOTE = 12;
const int MAX_NOTE = 131;
const int TABLE_LENGTH = 1024;

enum class SynthType { sine, sawtooth, square, triangle };

class Synth {
    static float *cSineWaveTable;
    static float *cSawtoothWaveTable;
    static float *cSquareWaveTable;
    static float *cTriangleWaveTable;
    SynthType mType;
    float mAmplitude;
    float mCurPhase;
    float mCurTime;
    int mPitch;
    Envelope mEnvelope;

  public:
    Synth(float amp);
    // main controls
    void noteOn(int pitch);
    void noteOff();
    // workhorse routine
    void addSamples(float *samples, long length);
    // getters, setters
    float amplitude() { return mAmplitude; }
    void amplitude(float v) { mAmplitude = v; }
    int pitch() { return mPitch; }
    SynthType type() { return mType; }
    void type(SynthType v) { mType = v; }
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