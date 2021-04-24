#include "synth.h"
#include <algorithm>
#include <iostream>

static float getFrequency(float note)
{
    // FIXME redo note indexes to match midi 69 = A4, not 57
    // Calculate pitch from note value.
    // offset note by 57 halfnotes to get correct pitch from the range we have
    // chosen for the notes.
    float p = pow(CHROMATIC_BASE, note - 57);
    p *= 440;
    return p;
}

#ifndef NDEBUG
void reportTableMinMax(float *waveTable, std::string name)
{
    float mn = 0.0f, mx = 0.0f;
    for (int i = 0; i < TABLE_LENGTH; i++) {
        mn = std::min(waveTable[i], mn);
        mx = std::max(waveTable[i], mx);
    }
    // FIXME? do we want to rescale to [-1.0,1.0]?
    std::cout << name << " min=" << mn << " max=" << mx << "\n";
}
#endif

// Generate a float wave tables with TABLE_LENGTH samples.
// This table will be used to produce the notes.
// Different notes will be created by stepping through
// the table at different intervals (phase).
static float *generateSineWaveTable()
{
    float *waveTable = new float[TABLE_LENGTH];
    float phaseInc = (2.0f * (float)M_PI) / (float)TABLE_LENGTH;
    float phase = 0;
    for (int i = 0; i < TABLE_LENGTH; i++) {
        waveTable[i] = sin(phase);
        phase += phaseInc;
    }
    return waveTable;
}

static float *generateSawtoothWaveTable()
{
    float *waveTable = new float[TABLE_LENGTH];
    memset(waveTable, 0, sizeof(float) * TABLE_LENGTH);
    float numOctaves = (int)(SAMPLE_RATE / 2.0 / 440.0);
    for (int octave = 1; octave < numOctaves; octave++) {
        float phaseInc = (octave * 2.0f * (float)M_PI) / (float)TABLE_LENGTH;
        float phase = 0;
        float sign = (octave & 1) ? -1.0f : 1.0f;
        for (int i = 0; i < TABLE_LENGTH; i++) {
            waveTable[i] += (sign * sin(phase) / octave) * (2.0f / (float)M_PI);
            phase += phaseInc;
        }
    }
#ifndef NDEBUG
    reportTableMinMax(waveTable, "SAW");
#endif
    return waveTable;
}

static float *generateSquareWaveTable()
{
    float *waveTable = new float[TABLE_LENGTH];
    memset(waveTable, 0, sizeof(float) * TABLE_LENGTH);
    float numOctaves = (int)(SAMPLE_RATE / 2.0 / 440.0);
    for (int octave = 1; octave < numOctaves; octave += 2) {
        float phaseInc = (octave * 2.0f * (float)M_PI) / (float)TABLE_LENGTH;
        float phase = 0;
        for (int i = 0; i < TABLE_LENGTH; i++) {
            waveTable[i] += (sin(phase) / octave) * (4.0f / (float)M_PI);
            phase += phaseInc;
        }
    }
#ifndef NDEBUG
    reportTableMinMax(waveTable, "SQUARE");
#endif
    return waveTable;
}
static float *generateTriangleWaveTable()
{
    float *waveTable = new float[TABLE_LENGTH];
    memset(waveTable, 0, sizeof(float) * TABLE_LENGTH);
    float numOctaves = (int)(SAMPLE_RATE / 2.0 / 440.0);
    for (int octave = 1, i = 0; octave < numOctaves; octave += 2, i++) {
        float phaseInc = (octave * 2.0f * (float)M_PI) / (float)TABLE_LENGTH;
        float phase = 0;
        float sign = (i & 1) ? -1.0f : 1.0f;
        for (int i = 0; i < TABLE_LENGTH; i++) {
            waveTable[i] += (sign * sin(phase) / (octave*octave)) * (8.0f / ((float)M_PI*(float)M_PI));
            phase += phaseInc;
        }
    }
#ifndef NDEBUG
    reportTableMinMax(waveTable, "TRIANGLE");
#endif
    return waveTable;
}

float *Synth::cSineWaveTable = generateSineWaveTable();
float *Synth::cSawtoothWaveTable = generateSawtoothWaveTable();
float *Synth::cSquareWaveTable = generateSquareWaveTable();
float *Synth::cTriangleWaveTable = generateTriangleWaveTable();

Synth::Synth(float amp) : mAmplitude(amp)
{
    mType = SynthType::sine;
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
    float phase_inc =
        (getFrequency((float)mPitch) / SAMPLE_RATE) * TABLE_LENGTH;

    // loop through the buffer and write samples.
    float waveSample = 0.0f;
    for (int i = 0; i < length; i += 2) {
        switch (mType) {
        case SynthType::sine:
            waveSample = Synth::cSineWaveTable[(int)mCurPhase];
            break;
        case SynthType::sawtooth:
            waveSample = Synth::cSawtoothWaveTable[(int)mCurPhase];
            break;
        case SynthType::square:
            waveSample = Synth::cSquareWaveTable[(int)mCurPhase];
            break;
        case SynthType::triangle:
            waveSample = Synth::cTriangleWaveTable[(int)mCurPhase];
            break;
        }
        float envAmp = mEnvelope.amplitude(mCurTime);
        float sample = mAmplitude * envAmp * (float)waveSample; // scale volume.
        samples[i] += sample;                                   // left channel
        samples[i + 1] += sample;                               // right channel
        mCurTime += SAMPLE_PERIOD;
        mCurPhase += phase_inc;
        if (mCurPhase >= TABLE_LENGTH) {
            mCurPhase = mCurPhase - TABLE_LENGTH;
        }
    }
}
