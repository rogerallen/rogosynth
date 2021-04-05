#include "synth.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

Synth::Synth(SDL_AudioDeviceID &audioDevice, SDL_AudioSpec &audioSpec)
{
    mAudioDevice = audioDevice;
    mAudioSpec = audioSpec;

    mSineWaveTable = new int16_t[TABLE_LENGTH];
    buildSineWaveTable();

    mEnvelopeCursor = 0;
    mEnvelopeSpeedScale = 1;
    mEnvelopeData[0] = 1.0;
    mEnvelopeData[1] = 0.5;
    mEnvelopeData[2] = 0.5;
    mEnvelopeData[3] = 0.0;
    mEnvelopeIncrementBase = 1.0 / ((double)audioSpec.freq / 2.0);

    mNote = -1; // negative when off
    mOctave = 2;
    mKeyPressed = false;

    mPhase = 0;
    mCurrentAmp = 0;
    mSmoothingAmpSpeed = 0.01;
}

Synth::~Synth() { delete[] mSineWaveTable; }

// Generate a 16bit signed integer sinewave table with 1024 samples.
// This table will be used to produce the notes.
// Different notes will be created by stepping through
// the table at different intervals (phase).
void Synth::buildSineWaveTable()
{
    double phase_increment = (2.0f * M_PI) / (double)TABLE_LENGTH;
    double current_phase = 0;
    for (int i = 0; i < TABLE_LENGTH; i++) {
        int sample = (int)(sin(current_phase) * INT16_MAX);
        mSineWaveTable[i] = (int16_t)sample;
        current_phase += phase_increment;
    }
}

void Synth::noteOn(int note)
{
    int curNote = mNote;

    mKeyPressed = true;
    mNote = note;
    mNote += (mOctave * 12);
    if (note < MIN_NOTE) {
        mNote = MIN_NOTE;
    }
    else if (note > MAX_NOTE) {
        mNote = MAX_NOTE;
    }
    //if (mNote != curNote) {
#ifndef NDEBUG
    std::cout << "noteOn " << mNote << "\n";
#endif
    mEnvelopeCursor = 0; // new note restarts envelope
    //}
}

void Synth::noteOff()
{
    mKeyPressed = false;
#ifndef NDEBUG
    std::cout << "noteOff\n";
#endif
}

void Synth::writeSamples(int16_t *s_byteStream, long begin, long end,
                         long length)
{
    if (mNote > 0) {
        double d_sample_rate = mAudioSpec.freq;
        double d_table_length = TABLE_LENGTH;
        double d_note = mNote;

        // get correct phase increment for note depending on sample rate and
        // table length.
        double phase_increment =
            (getPitch(d_note) / d_sample_rate) * d_table_length;

        // loop through the buffer and write samples.
        for (int i = 0; i < length; i += 2) {
            mPhase += phase_increment;
            int phase_int = (int)mPhase;
            if (mPhase >= TABLE_LENGTH) {
                double diff = mPhase - TABLE_LENGTH;
                mPhase = diff;
                phase_int = (int)diff;
            }

            if (phase_int < TABLE_LENGTH && phase_int > -1) {
                if (s_byteStream != NULL) {
                    int16_t sample = mSineWaveTable[phase_int];
                    double target_amp = updateEnvelope();
                    // move current amp towards target amp for a smoother
                    // transition.
                    if (mCurrentAmp < target_amp) {
                        mCurrentAmp += mSmoothingAmpSpeed;
                        if (mCurrentAmp > target_amp) {
                            mCurrentAmp = target_amp;
                        }
                    }
                    else if (mCurrentAmp > target_amp) {
                        mCurrentAmp -= mSmoothingAmpSpeed;
                        if (mCurrentAmp < target_amp) {
                            mCurrentAmp = target_amp;
                        }
                    }
                    sample *= mCurrentAmp;                // scale volume.
                    s_byteStream[i + begin] = sample;     // left channel
                    s_byteStream[i + begin + 1] = sample; // right channel
                }
            }
        }
    }
}

double Synth::getPitch(double note)
{
    // Calculate pitch from note value.
    // offset note by 57 halfnotes to get correct pitch from the range we have
    // chosen for the notes.
    double p = pow(CHROMATIC_RATIO, note - 57);
    p *= 440;
    return p;
}

// advance envelope cursor and return the target amplitude value.
double Synth::updateEnvelope(void)
{
    double amp = 0;
    if (mKeyPressed && mEnvelopeCursor < 3 && mEnvelopeCursor > 2) {
        // if a note key is longpressed and cursor is in range, stay for
        // sustain.
        amp = getEnvelopeAmpByNode(2, mEnvelopeCursor);
    }
    else {
        double speed_multiplier = pow(2, mEnvelopeSpeedScale);
        double cursor_inc = mEnvelopeIncrementBase * speed_multiplier;
        mEnvelopeCursor += cursor_inc;
        if (mEnvelopeCursor < 1) {
            amp = getEnvelopeAmpByNode(0, mEnvelopeCursor);
        }
        else if (mEnvelopeCursor < 2) {
            amp = getEnvelopeAmpByNode(1, mEnvelopeCursor);
        }
        else if (mEnvelopeCursor < 3) {
            amp = getEnvelopeAmpByNode(2, mEnvelopeCursor);
        }
        else {
            amp = mEnvelopeData[3];
        }
    }
    return amp;
}

// interpolate amp value for the current cursor position.
double Synth::getEnvelopeAmpByNode(int base_node, double cursor)
{
    double n1 = base_node;
    double n2 = base_node + 1;
    double relative_cursor_pos = (cursor - n1) / (n2 - n1);
    double amp_diff = (mEnvelopeData[base_node + 1] - mEnvelopeData[base_node]);
    double amp = mEnvelopeData[base_node] + (relative_cursor_pos * amp_diff);
    return amp;
}