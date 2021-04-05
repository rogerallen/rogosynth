#ifndef ROGOSYNTH_SYNTH_H
#define ROGOSYNTH_SYNTH_H

#include <SDL.h>

class Synth {
    SDL_AudioDeviceID mAudioDevice;
    SDL_AudioSpec mAudioSpec;

    const int TABLE_LENGTH = 1024;
    int16_t *mSineWaveTable;

    double mEnvelopeCursor;
    double mEnvelopeSpeedScale;
    double mEnvelopeData[4];  // ADSR (fixme)
    double mEnvelopeIncrementBase;

    const int MIN_NOTE = 12;
    const int MAX_NOTE = 131;

    int mNote;
    int mOctave;
    bool mKeyPressed;

    double mPhase;
    double mCurrentAmp;
    double mSmoothingAmpSpeed;

    void buildSineWaveTable();
    const double CHROMATIC_RATIO = 1.059463094359295264562;
    double getPitch(double note);
    double updateEnvelope();
    double getEnvelopeAmpByNode(int base_node, double cursor);

  public:
    Synth(SDL_AudioDeviceID &audioDevice, SDL_AudioSpec &audioSpec);
    ~Synth();
    void noteOn(int note);
    void noteOff();
    void writeSamples(int16_t *s_byteStream, long begin, long end, long length);
};
#endif