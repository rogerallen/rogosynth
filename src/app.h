#ifndef ROGOSYNTH_APP_H
#define ROGOSYNTH_APP_H

#define _USE_MATH_DEFINES
#include <cmath>

#include "appGL.h"
#include "appWindow.h"
#include "rogosynth.h"

#include <GL/glew.h>
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <string>

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

    RogoSynth *mRogoSynth;
    float *mAudioBuffer;

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
