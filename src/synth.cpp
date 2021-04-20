#include "synth.h"
#include <algorithm>
#include <iostream>

static float getFrequency(float note)
{
    // Calculate pitch from note value.
    // offset note by 57 halfnotes to get correct pitch from the range we have
    // chosen for the notes.
    float p = pow(CHROMATIC_BASE, note - 57);
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
    float phaseInc = (2.0f * (float)M_PI) / (float)TABLE_LENGTH;
    float phase = 0;
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
    mCurPhase = 0;
    mCurTime = 0.0;
    mPitch = MIN_NOTE;
    mEnvelope.attack(0.2f);
    mEnvelope.decay(0.2f);
    mEnvelope.sustain(0.8f);
    mEnvelope.release(0.2f);
}

void Synth::noteOn(int pitch)
{
    mPitch = std::clamp(pitch, MIN_NOTE, MAX_NOTE);
    mEnvelope.noteOn(mCurTime);
#ifndef NDEBUG
    std::cout << "noteOn  " << mPitch << "\n";
#endif
}

void Synth::noteOff()
{
    mEnvelope.noteOff(mCurTime);
#ifndef NDEBUG
    std::cout << "noteOff " << mPitch << "\n";
#endif
}

// add samples to the samples buffer
void Synth::addSamples(float *samples, long length)
{
    // get correct phase increment for note depending on sample rate and
    // table length.
    float phase_inc = (getFrequency((float)mPitch) / SAMPLE_RATE) * TABLE_LENGTH;

    // loop through the buffer and write samples.
    for (int i = 0; i < length; i += 2) {
        float waveSample =
            (float)Synth::cSineWaveTable[(int)mCurPhase] / INT16_MAX;
        float amp = mEnvelope.amplitude(mCurTime);
        float sample = amp * (float)waveSample; // scale volume.
        samples[i] += sample;                     // left channel
        samples[i + 1] += sample;                 // right channel
        mCurTime += TIME_INC;
        mCurPhase += phase_inc;
        if (mCurPhase >= TABLE_LENGTH) {
            mCurPhase = mCurPhase - TABLE_LENGTH;
        }
    }
}
