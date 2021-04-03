cmake_minimum_required(VERSION 3.14)

if(WIN32)
    # On Linux you can apt-get install these to system locations
    set(SDL2_DIR  ${CMAKE_SOURCE_DIR}/external/SDL2-2.0.12)
    set(GLEW_ROOT ${CMAKE_SOURCE_DIR}/external/glew-2.1.0)
endif()
set(GLM_DIR    ${CMAKE_SOURCE_DIR}/external/glm-0.9.9.8)
set(IMGUI_ROOT ${CMAKE_SOURCE_DIR}/external/imgui-1.76)

# SDL2, glew, glm, ImGui
add_subdirectory("external")

project(rogosynth
    VERSION     0.1
    DESCRIPTION "SDL2 ImGui Synth"
    LANGUAGES   CXX
)

# IMGUI
set(IMGUI_SOURCES ${IMGUI_ROOT}/imgui.cpp ${IMGUI_ROOT}/imgui_draw.cpp ${IMGUI_ROOT}/imgui_widgets.cpp)
set(IMGUI_IMPL_SOURCES ${IMGUI_ROOT}/examples/imgui_impl_sdl.cpp ${IMGUI_ROOT}/examples/imgui_impl_opengl3.cpp)

add_executable(rogosynth src/main.cpp src/app.cpp src/appGL.cpp
    ${IMGUI_SOURCES} ${IMGUI_IMPL_SOURCES})

target_include_directories(rogosynth PUBLIC "${IMGUI_ROOT}")
target_include_directories(rogosynth PUBLIC "${IMGUI_ROOT}/examples")

target_compile_features(rogosynth PUBLIC cxx_std_14)

target_include_directories(rogosynth PUBLIC "${GLM_DIR}")

# Windows prereq: get dev libraries from https://www.libsdl.org/download-2.0.php
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
# workaround for CMP0004 issue with the SDL2 libraries on Linux
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
target_link_libraries(rogosynth ${SDL2_LIBRARIES})

# OpenGL
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
if(NOT TARGET OpenGL::OpenGL)
  target_link_libraries(rogosynth OpenGL::GL)
  target_include_directories(rogosynth PUBLIC OpenGL::GL)
else()
  target_link_libraries(rogosynth OpenGL::OpenGL)
  target_include_directories(rogosynth PUBLIC OpenGL::OpenGL)
endif()

# GL Extension Wrangler (GLEW) get dev libraries from http://glew.sourceforge.net/
find_package(GLEW REQUIRED)
target_link_libraries(rogosynth "${GLEW_LIBRARIES}")
target_include_directories(rogosynth PUBLIC "${GLEW_INCLUDE_DIRS}")

# Windows: copy the dll as a post-build step
if(WIN32)
    add_custom_command(TARGET rogosynth POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL2_DIR}/lib/x64/SDL2.dll"
            $<TARGET_FILE_DIR:rogosynth>)
    add_custom_command(TARGET rogosynth POST_BUILD
         COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLEW_DLL_DIR}/glew32.dll"
            $<TARGET_FILE_DIR:rogosynth>)
endif()
