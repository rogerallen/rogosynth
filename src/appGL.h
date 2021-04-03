#ifndef ROGOSYNTH_APP_GL_H
#define ROGOSYNTH_APP_GL_H

#include "appWindow.h"

#include "glm/ext.hpp"
#include "glm/glm.hpp"
#include <GL/glew.h>

#include <string>

void GLAPIENTRY
MessageCallback(GLenum source,
                GLenum type,
                GLuint id,
                GLenum severity,
                GLsizei length,
                const GLchar *message,
                const void *userParam);

class AppGL {
    AppWindow *mWindow;
    glm::mat4 mCameraToView;
  public:
    AppGL(AppWindow *appWindow, unsigned maxWidth, unsigned maxHeight)
    {
#ifndef NDEBUG
        std::cout << "maxWidth,height = " << maxWidth << "," << maxHeight << std::endl;
#endif
        mWindow = appWindow;
        glClearColor(1.0, 1.0, 0.5, 0.0);
        mCameraToView = glm::mat4(1.0f);
#ifdef WIN32
        const std::string pathSep = "\\";
#else
        const std::string pathSep = "/";
#endif
#ifndef NDEBUG
        //std::cout << "source directory = " << shaderPath << std::endl;
#endif
        // During init, enable debug output
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(MessageCallback, 0);
    }
    ~AppGL()
    {
    }
    void handleResize()
    {
        glViewport(0, 0, mWindow->width(), mWindow->height());
        if (mWindow->resized()) {
            mWindow->resizeHandled();
            // anchor viewport to upper left corner (0, 0) to match the anchor on
            // the sharedTexture surface. See picture above.
            float xpos = 1.0f, ypos = 1.0f;
            if (mWindow->width() >= mWindow->height()) {
                ypos = (float)mWindow->height() / (float)mWindow->width();
            }
            else {
                xpos = (float)mWindow->width() / (float)mWindow->height();
            }
            mCameraToView = glm::ortho(0.0f, xpos, ypos, 0.0f);
        }
    }
    void render()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

#endif
