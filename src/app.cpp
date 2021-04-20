#include "app.h"

#include "examples/imgui_impl_opengl3.h"
#include "examples/imgui_impl_sdl.h"
#include "imgui.h"

#ifdef WIN32
// don't interfere with std::min,max
#define NOMINMAX
// https://seabird.handmade.network/blogs/p/2460-be_aware_of_high_dpi
#pragma comment(lib, "Shcore.lib")

#include <ShellScalingAPI.h>
#include <comdef.h>
#include <windows.h>
#endif

// Go from C-style thread call to C++ method.
// userdata points to the App class.
static void staticAudioCallback(void *userdata, Uint8 *byte_stream,
                                int byte_stream_length)
{
    static_cast<App *>(userdata)->audioCallback(byte_stream,
                                                byte_stream_length);
}

App::App()
{
    mAppWindow = nullptr;
    mAppGL = nullptr;
    mSDLWindow = nullptr;
    mSDLGLContext = nullptr;

    for (int i = 0; i < NUM_SYNTHS; i++) {
        mSynths[i] = new Synth(SYNTH_AMPLITUDE);
    }
    mAudioBuffer = nullptr;
    mAudioBufferSize = 0;

    mSwitchFullscreen = false;
    mIsFullscreen = false;
    mMouseDown = false;
    mShowGUI = false;

    mPrevWindowWidth = mPrevWindowHeight = -1;
    mPrevWindowX = mPrevWindowY = -1;

    mMonitorWidth = mMonitorHeight = -1;

    mMouseStartX = mMouseStartY = mMouseX = mMouseY = mCenterStartX =
        mCenterStartY = -1;
}

App::~App()
{
    for (int i = 0; i < NUM_SYNTHS; i++) {
        delete mSynths[i];
    }
    delete[] mAudioBuffer;
}

void App::run()
{
#ifndef NDEBUG
    std::cout << "Rogosynth" << std::endl;
    std::cout << "Versions-------------+-------" << std::endl;
    SDL_version compiled;
    SDL_version linked;

    SDL_VERSION(&compiled);
    SDL_GetVersion(&linked);
    std::cout << "SDL compiled version | " << int(compiled.major) << "."
              << int(compiled.minor) << "." << int(compiled.patch) << std::endl;
    std::cout << "SDL linked version   | " << int(linked.major) << "."
              << int(linked.minor) << "." << int(linked.patch) << std::endl;

    std::cout << "GLM version          | " << GLM_VERSION << std::endl;
#endif

    if (!init()) {
        loop();
    }
    cleanup();
}

void App::cleanup()
{
#ifndef NDEBUG
    std::cout << "Exiting..." << std::endl;
#endif

    SDL_CloseAudioDevice(mAudioDevice);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyWindow(mSDLWindow);
}

// initialize Window, AppGL classes
// return true on error
bool App::init()
{
    if (initWindow()) {
        return true;
    }
    mAppGL = new AppGL(mAppWindow, mMonitorWidth, mMonitorHeight);
    if (initAudio()) {
        return true;
    }
    return false;
}

// initialize SDL2 window
// return true on error
bool App::initWindow()
{
#ifdef WIN32
    SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

    // Initialize SDL Video
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL video" << std::endl;
        return true;
    }

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    mMonitorWidth = DM.w;
    mMonitorHeight = DM.h;

    int startDim = std::min(mMonitorWidth, mMonitorHeight) / 2;
    mAppWindow = new AppWindow(startDim, startDim);

    // Create main window
    mSDLWindow = SDL_CreateWindow("rogosynth", SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED, startDim, startDim,
                                  SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE |
                                      SDL_WINDOW_ALLOW_HIGHDPI);
    if (mSDLWindow == NULL) {
        std::cerr << "Failed to create main window" << std::endl;
        SDL_Quit();
        return true;
    }

    // Initialize rendering context
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
                        SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    mSDLGLContext = SDL_GL_CreateContext(mSDLWindow);
    if (mSDLGLContext == NULL) {
        std::cerr << "Failed to create GL context" << std::endl;
        SDL_DestroyWindow(mSDLWindow);
        SDL_Quit();
        return true;
    }

    SDL_GL_SetSwapInterval(1); // Use VSYNC

    // Initialize GL Extension Wrangler (GLEW)
    GLenum err;
    glewExperimental = GL_TRUE; // Please expose OpenGL 3.x+ interfaces
    err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Failed to init GLEW" << std::endl;
        SDL_GL_DeleteContext(mSDLGLContext);
        SDL_DestroyWindow(mSDLWindow);
        SDL_Quit();
        return true;
    }

    // initialize imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // setup Dear ImGui style
    ImGui::StyleColorsDark();

    // setup platform/renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(mSDLWindow, mSDLGLContext);
    ImGui_ImplOpenGL3_Init("#version 330");

    // colors are set in RGBA, but as float
    // ImVec4 background = ImVec4(35/255.0f, 35/255.0f, 35/255.0f, 1.00f);

#ifndef NDEBUG
    int major, minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    std::cout << "OpenGL version       | " << major << "." << minor
              << std::endl;
    std::cout << "GLEW version         | " << glewGetString(GLEW_VERSION)
              << std::endl;
    std::cout << "---------------------+-------" << std::endl;
#endif

    return false;
}

#define BUFFER_SIZE 4096 // must be power of two

// initialize SDL2 audio
// return true on error
bool App::initAudio()
{
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
    SDL_AudioSpec want;
    SDL_zero(want);
    SDL_zero(mAudioSpec);

    // desired audio spec
    want.freq = SAMPLE_RATE;
    want.format = AUDIO_S16LSB;
    want.channels = 2;
    want.samples = BUFFER_SIZE;
    want.userdata = this;
    want.callback = staticAudioCallback;

    mAudioDevice = SDL_OpenAudioDevice(NULL, 0, &want, &mAudioSpec, 0);

    if (mAudioDevice == 0) {
        std::cerr << "ERROR: Failed to open audio: " << SDL_GetError()
                  << std::endl;
        return true;
    }

    if (mAudioSpec.freq != want.freq) {
        std::cerr << "ERROR: Couldn't get requested audio freq." << std::endl;
        return true;
    }
    if (mAudioSpec.format != want.format) {
        std::cerr << "ERROR: Couldn't get requested audio format." << std::endl;
        return true;
    }
    if (mAudioSpec.channels != want.channels) {
        std::cerr << "ERROR: Couldn't get requested audio channels."
                  << std::endl;
        return true;
    }
    if (mAudioSpec.samples != want.samples) {
        std::cerr << "ERROR: Couldn't get requested audio samples."
                  << std::endl;
        return true;
    }

#ifndef NDEBUG
    std::cout << "audioSpec:\n";
    std::cout << "     freq: " << mAudioSpec.freq << "\n";
    std::cout << "   format: AUDIO_S16LSB\n";
    std::cout << " channels: " << (int)mAudioSpec.channels << "\n";
    std::cout << "  samples: " << mAudioSpec.samples << "\n";
    std::cout << "     size: " << mAudioSpec.size << "\n";
#endif

    SDL_PauseAudioDevice(mAudioDevice, 0); // unpause audio.

    return false;
}

void App::loop()
{
#ifndef NDEBUG
    std::cout << "Running..." << std::endl;
#endif
    bool running = true;
    static Uint32 lastFrameEventTime = 0;
    const Uint32 debounceTime = 100; // 100ms

    while (running) {
        Uint32 curTime = SDL_GetTicks();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {

            ImGui_ImplSDL2_ProcessEvent(&event);
            ImGuiIO &io = ImGui::GetIO();
            if (io.WantCaptureKeyboard || io.WantCaptureMouse) {
                break;
            }

            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE) {
                running = false;
                break;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_TAB:
                    mShowGUI = !mShowGUI;
                    break;
                case SDLK_F1: // F1 == FULLSCREEN
                    // getting double f events when we switch to fullscreen
                    // only on desktop linux!  So, let's slow this down to
                    // "debounce" those switches
                    if (curTime > lastFrameEventTime + debounceTime) {
                        mSwitchFullscreen = true;
                        lastFrameEventTime = curTime;
                    }
                    break;
                default:
                    int pitch = symToPitch(event.key.keysym.sym);
                    if ((pitch > -1) && (event.key.repeat == 0)) {
                        bool foundSynth = false;
                        for (int i = 0; i < NUM_SYNTHS; i++) {
                            if (!mSynths[i]->active()) {
                                foundSynth = true;
#ifndef NDEBUG
                                std::cout << i << ": ";
#endif
                                mSynths[i]->noteOn(pitch);
                                break;
                            }
                        }
                        if (!foundSynth) {
                            std::cout << "ERROR: NOT ENOUGH POLYPHONY!\n";
                        }
                    }
                    break;
                }
            }
            else if (event.type == SDL_KEYUP) {
                int pitch = symToPitch(event.key.keysym.sym);
                if (pitch > -1) {
                    bool foundSynth = false;
                    for (int i = 0; i < NUM_SYNTHS; i++) {
                        if (mSynths[i]->pitch() == pitch) {
                            if (mSynths[i]->releasing()) {
                                std::cout
                                    << "note releasing already. keep looking\n";
                                continue;
                            }
                            foundSynth = true;
#ifndef NDEBUG
                            std::cout << i << ": ";
#endif
                            mSynths[i]->noteOff();
                            break;
                        }
                    }
                    if (!foundSynth) {
                        std::cout << "ERROR: DID NOT FIND NOTE OFF SYNTH!\n";
                    }
                }
            }
            else if (event.type == SDL_WINDOWEVENT) {
                if ((event.window.event == SDL_WINDOWEVENT_RESIZED) ||
                    (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)) {
                    resize(event.window.data1, event.window.data2);
                }
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mMouseStartX = mMouseX = event.button.x;
                    mMouseStartY = mMouseY = event.button.y;
                    mMouseDown = true;
                }
            }
            else if (event.type == SDL_MOUSEMOTION) {
                mMouseX = event.motion.x;
                mMouseY = event.motion.y;
            }
            else if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    mMouseDown = false;
                }
            }
            else if (event.type == SDL_MOUSEWHEEL) {
                /* FIXME */
            }
        }

        update();
        mAppGL->render();

        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame(mSDLWindow);
            ImGui::NewFrame();
            if (mShowGUI) {
                ImGui::Begin("Information", NULL,
                             ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Framerate  : %.1f ms or %.1f Hz",
                            1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
                ImGui::End();
            }
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        SDL_GL_SwapWindow(mSDLWindow);
    }
}

void App::update()
{
    mAppGL->handleResize();

    // handle fullscreen
    if (mSwitchFullscreen) {
#ifndef NDEBUG
        std::cout << "switch fullscreen ";
#endif
        mSwitchFullscreen = false;
        if (mIsFullscreen) { // switch to windowed
#ifndef NDEBUG
            std::cout << "to windowed" << std::endl;
#endif
            mIsFullscreen = false;
            SDL_SetWindowFullscreen(mSDLWindow, 0);
            SDL_RestoreWindow(mSDLWindow); // Seemingly required for Jetson
            SDL_SetWindowSize(mSDLWindow, mPrevWindowWidth, mPrevWindowHeight);
            SDL_SetWindowPosition(mSDLWindow, mPrevWindowX, mPrevWindowY);
        }
        else { // switch to fullscreen
#ifndef NDEBUG
            std::cout << std::endl;
#endif
            mIsFullscreen = true;
            mPrevWindowWidth = mAppWindow->width();
            mPrevWindowHeight = mAppWindow->height();
            SDL_GetWindowPosition(mSDLWindow, &mPrevWindowX, &mPrevWindowY);
            SDL_SetWindowSize(mSDLWindow, mMonitorWidth, mMonitorHeight);
            SDL_SetWindowFullscreen(
                mSDLWindow,
                SDL_WINDOW_FULLSCREEN_DESKTOP); // "fake" fullscreen
        }
    }
}

void App::resize(unsigned width, unsigned height)
{
    if (width > 0 && height > 0 &&
        (mAppWindow->width() != width || mAppWindow->height() != height)) {
        mAppWindow->width(width);
        mAppWindow->height(height);
    }
}

int App::symToPitch(SDL_Keycode sym)
{
    int pitch = -1;
    switch (sym) {
    case SDLK_z:
        pitch = 12;
        break;
    case SDLK_s:
        pitch = 13;
        break;
    case SDLK_x:
        pitch = 14;
        break;
    case SDLK_d:
        pitch = 15;
        break;
    case SDLK_c:
        pitch = 16;
        break;
    case SDLK_v:
        pitch = 17;
        break;
    case SDLK_g:
        pitch = 18;
        break;
    case SDLK_b:
        pitch = 19;
        break;
    case SDLK_h:
        pitch = 20;
        break;
    case SDLK_n:
        pitch = 21;
        break;
    case SDLK_j:
        pitch = 22;
        break;
    case SDLK_m:
        pitch = 23;
        break;
    case SDLK_COMMA:
        pitch = 24;
        break;
    case SDLK_l:
        pitch = 25;
        break;
    case SDLK_PERIOD:
        pitch = 26;
        break;

        // upper keyboard
    case SDLK_q:
        pitch = 24;
        break;
    case SDLK_2:
        pitch = 25;
        break;
    case SDLK_w:
        pitch = 26;
        break;
    case SDLK_3:
        pitch = 27;
        break;
    case SDLK_e:
        pitch = 28;
        break;
    case SDLK_r:
        pitch = 29;
        break;
    case SDLK_5:
        pitch = 30;
        break;
    case SDLK_t:
        pitch = 31;
        break;
    case SDLK_6:
        pitch = 32;
        break;
    case SDLK_y:
        pitch = 33;
        break;
    case SDLK_7:
        pitch = 34;
        break;
    case SDLK_u:
        pitch = 35;
        break;
    case SDLK_i:
        pitch = 36;
        break;
    case SDLK_9:
        pitch = 37;
        break;
    case SDLK_o:
        pitch = 38;
        break;
    case SDLK_0:
        pitch = 39;
        break;
    case SDLK_p:
        pitch = 40;
        break;
    }
    if (pitch > 0) {
        pitch += 2 * 12;
    }
    return pitch;
}

void App::audioCallback(Uint8 *byte_stream, int byte_stream_size_in_bytes)
{
    // zero the buffer
    memset(byte_stream, 0, byte_stream_size_in_bytes);

    // if necessary, create the mAudioBuffer
    int sizeInSamples = byte_stream_size_in_bytes / 2; // 16bit samples
    if (mAudioBufferSize < sizeInSamples) {
#ifndef NDEBUG
        std::cout << "Create audio blend buffer of " << sizeInSamples
                  << " samples\n";
#endif
        mAudioBufferSize = sizeInSamples;
        if (mAudioBuffer != nullptr) {
            delete[] mAudioBuffer;
        }
        mAudioBuffer = new float[sizeInSamples];
    }
    memset(mAudioBuffer, 0, sizeof(float) * sizeInSamples);

    int numActiveSynths = 0;
    for (int i = 0; i < NUM_SYNTHS; i++) {
        if (mSynths[i]->active()) {
            numActiveSynths++;
        }
        // always call addSamples so time & phase are consistent
        mSynths[i]->addSamples(mAudioBuffer, sizeInSamples);
    }
    // FIXME -- eventually need a compressor to keep mAudioBuffer [-1,1]
    if (numActiveSynths > 0) {
        Sint16 *short_stream = (Sint16 *)byte_stream;
        for (int i = 0; i < sizeInSamples; i++) {
            short_stream[i] = (Sint16)(mAudioBuffer[i] * (float)INT16_MAX);
        }
    }
}
