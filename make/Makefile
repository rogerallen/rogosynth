#
# I've no idea how to get nvc++ to work with VS Code.
# So, just go with a Makefile
#
USE_GCC=0
USE_MINITRACE=1

ifeq ($(USE_GCC), 1)
  CC = /bin/x86_64-linux-gnu-gcc-9
  CXX = /bin/x86_64-linux-gnu-g++-9
  CFLAGS = -Wall -O3
  CXXFLAGS = -Wall -std=c++17 -O3
else
  CC = nvcc
  CXX = nvc++
  CFLAGS = -O3
  CXXFLAGS = -std=c++17 -O3
endif

GLM_ROOT   = ../external/glm-0.9.9.8
IMGUI_ROOT = ../external/imgui-1.76
SDL2_ROOT  = /usr/include/SDL2
IMGUI_SRC  = $(IMGUI_ROOT)/imgui.cpp $(IMGUI_ROOT)/imgui_draw.cpp \
   $(IMGUI_ROOT)/imgui_widgets.cpp \
   $(IMGUI_ROOT)/examples/imgui_impl_sdl.cpp $(IMGUI_ROOT)/examples/imgui_impl_opengl3.cpp

ROGOSYNTH_C_SRC = ../src/audio.c \
    ../src/sndfilter/biquad.c ../src/sndfilter/compressor.c ../src/sndfilter/reverb.c

ROGOSYNTH_CXX_SRC = ../src/main.cpp ../src/app.cpp ../src/appGL.cpp \
    ../src/rogosynth.cpp ../src/synthvoice.cpp \
    $(IMGUI_SRC)

ifeq ($(USE_MINITRACE), 1)
ROGOSYNTH_C_SRC += ../src/minitrace/minitrace.c
CFLAGS += -DMTR_ENABLED
CXXFLAGS += -DMTR_ENABLED
endif

ROGOSYNTH_OBJS = $(ROGOSYNTH_C_SRC:.c=.o) $(ROGOSYNTH_CXX_SRC:.cpp=.o) 

ROGOSYNTH_INCS = -I$(IMGUI_ROOT) -I$(IMGUI_ROOT)/examples \
    -I$(GLM_ROOT) -I$(SDL2_ROOT)

LDFLAGS=-lSDL2 -lGLEW -lOpenGL -lm 

rogosynth: $(ROGOSYNTH_OBJS)
	$(CXX) -o $@ $(ROGOSYNTH_OBJS) $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^ $(ROGOSYNTH_INCS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^ $(ROGOSYNTH_INCS)

clean:
	rm -f rogosynth $(ROGOSYNTH_OBJS)