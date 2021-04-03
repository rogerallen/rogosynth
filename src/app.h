#ifndef ROGOSYNTH_APP_H
#define ROGOSYNTH_APP_H

#include "appGL.h"
#include "appWindow.h"
#include <GL/glew.h>
#include <SDL.h>
#include <algorithm>
#include <iostream>
#include <string>

class App {

    bool init();
    bool initWindow();
    void loop();
    void update();
    void cleanup();
    void resize(unsigned width, unsigned height);

    AppWindow *mAppWindow;
    AppGL *mAppGL;
    SDL_Window *mSDLWindow;
    SDL_GLContext mSDLGLContext;
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
    void run();
};
#endif
