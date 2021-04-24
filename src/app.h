#ifndef ROGOSYNTH_APP_H
#define ROGOSYNTH_APP_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "appGL.h"
#include "appWindow.h"
#include "compressor.h"
#include "lowpassfilter.h"
#include "reverb.h"
#include "synth.h"
#include <GL/glew.h>
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <string>

const int NUM_SYNTHS = 8;
const float SYNTH_AMPLITUDE = 1.0f/NUM_SYNTHS;

class App {

    bool init();
    bool initWindow();
    bool initAudio();
    void loop();
    void showGUI();
    void update();
    void cleanup();
    void resize(unsigned width, unsigned height);
    int symToPitch(SDL_Keycode sym);

    AppWindow *mAppWindow;
    AppGL *mAppGL;
    SDL_Window *mSDLWindow;
    SDL_GLContext mSDLGLContext;

    SDL_AudioSpec mAudioSpec;
    SDL_AudioDeviceID mAudioDevice;

    Synth *mSynths[NUM_SYNTHS];
    float mAudioBuffer[AUDIO_BUFFER_SAMPLES];
    float mPanPosition;
    Compressor *mCompressor;
    LowPassFilter *mLowPassFilter;
    Reverb *mReverb; // Needs to be on the heap, not the stack

    bool mSwitchFullscreen;
    bool mIsFullscreen;
    int mMonitorWidth, mMonitorHeight;
    int mPrevWindowWidth, mPrevWindowHeight;
    int mPrevWindowX, mPrevWindowY;
    bool mMouseDown;
    double mMouseStartX, mMouseStartY;
    double mMouseX, mMouseY;
    double mCenterStartX, mCenterStartY;
    bool mShowGUI;

  public:
    App();
    ~App();
    void run();
    void audioCallback(Uint8 *byte_stream, int byte_stream_length);
};
#endif
