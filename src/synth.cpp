#include "synth.h"
#include <algorithm>
#include <iostream>

static double getFrequency(double note)
{
    // Calculate pitch from note value.
    // offset note by 57 halfnotes to get correct pitch from the range we have
    // chosen for the notes.
    double p = pow(CHROMATIC_BASE, note - 57);
    p *= 440;
    return p;
}

static int16_t *generateSineWaveTable()
{
    int16_t *sineWaveTable = new int16_t[TABLE_LENGTH];

    // Generate a 16bit signed integer sinewave table with 1024 samples.
    // This table will be used to produce the notes.
    // Different notes will be created by stepping through
    // the table at different intervals (phase).
    double phaseInc = (2.0 * M_PI) / (double)TABLE_LENGTH;
    double phase = 0;
    for (int i = 0; i < TABLE_LENGTH; i++) {
        int sample = (int)(sin(phase) * INT16_MAX);
        sineWaveTable[i] = (int16_t)sample;
        phase += phaseInc;
    }

    return sineWaveTable;
}

int16_t *Synth::cSineWaveTable = generateSineWaveTable();

Synth::Synth()
{

    mEnvelope.attack(0.2);
    mEnvelope.decay(0.2);
    mEnvelope.sustain(0.8);
    mEnvelope.release(0.2);

    mNote = -1; // negative when off
    mOctave = 2;
    mKeyPressed = false;

    mCurPhase = 0;
    mCurTime = 0.0;
}

//Synth::~Synth() { }

void Synth::noteOn(int note)
{
    mKeyPressed = true;
    mNote = std::clamp(note + 12 * mOctave, MIN_NOTE, MAX_NOTE);
    mEnvelope.noteOn(mCurTime);
#ifndef NDEBUG
    std::cout << "noteOn " << mNote << "\n";
#endif
}

void Synth::noteOff()
{
    mKeyPressed = false;
    mEnvelope.noteOff(mCurTime);
#ifndef NDEBUG
    std::cout << "noteOff\n";
#endif
}

void Synth::writeSamples(int16_t *samples, long length)
{
    if (samples == NULL) {
        return;
    }

    if (mNote < 0) {
        return;
    }

    if (!mEnvelope.active(mCurTime)) {
        return;
    }

    // get correct phase increment for note depending on sample rate and
    // table length.
    double time_inc = 1.0 / SAMPLE_RATE;
    double phase_inc = (getFrequency(mNote) / SAMPLE_RATE) * TABLE_LENGTH;

    // loop through the buffer and write samples.
    for (int i = 0; i < length; i += 2) {
        mCurTime += time_inc;
        mCurPhase += phase_inc;
        if (mCurPhase >= TABLE_LENGTH) {
            mCurPhase = mCurPhase - TABLE_LENGTH;
        }
        int phase_int = (int)mCurPhase;
        int16_t sample = Synth::cSineWaveTable[phase_int];
        double amp = mEnvelope.amplitude(mCurTime);
        sample = (int16_t)(amp * (double)sample); // scale volume.
        samples[i] = sample;                      // left channel
        samples[i + 1] = sample;                  // right channel
    }
}

