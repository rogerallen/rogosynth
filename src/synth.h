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

class Synth {
    const int TABLE_LENGTH = 1024;
    int16_t *mSineWaveTable;
    double mCurPhase;
    double mCurTime;

    int mNote;
    int mOctave;
    bool mKeyPressed;

    Envelope mEnvelope;

    void buildSineWaveTable();
    double getFrequency(double note);


  public:
    Synth();
    ~Synth();
    void noteOn(int note);
    void noteOff();
    void writeSamples(int16_t *samples, long length);
};
#endif