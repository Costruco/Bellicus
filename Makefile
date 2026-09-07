# ---- Sources ----
SRCS = src/main.cpp \
       src/Game.cpp \
       src/FrameManager.cpp \
       src/TextureManager.cpp \
       src/Map.cpp \
       src/Vector2D.cpp \
       src/Collision.cpp \
       src/EntityComponentSystem/EntityComponentSystem.cpp

CC     = g++
CXXSTD = -std=c++20

# ---- Detect platform ----
ifeq ($(OS),Windows_NT)
    PLATFORM := windows
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        PLATFORM := macos
    else
        PLATFORM := linux
    endif
endif

# ---- Platform-specific settings ----
ifeq ($(PLATFORM),windows)
    SDL2_ROOT     ?= C:/msys64/ucrt64
    INCLUDE_PATHS  = -I./src -I$(SDL2_ROOT)/include/SDL2
    LIBRARY_PATHS  = -L$(SDL2_ROOT)/lib
    OBJ_NAME       = bin/Bellicus.exe
    COMPILER_FLAGS = $(CXXSTD) -static-libgcc -static-libstdc++

    PKG_LIBS := $(shell pkg-config --static --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf SDL2_gfx)
    PKG_LIBS := $(filter-out -mwindows,$(PKG_LIBS))

    # -mconsole forces the console subsystem back on, overriding SDL2's -mwindows
    LINKER_FLAGS = -static -lmingw32 -lSDL2main $(PKG_LIBS) -mconsole
else
    INCLUDE_PATHS  = -I./src $(shell pkg-config --cflags sdl2 SDL2_image SDL2_mixer SDL2_ttf SDL2_gfx)
    LIBRARY_PATHS  =
    OBJ_NAME       = bin/Bellicus
    COMPILER_FLAGS = $(CXXSTD)
    LINKER_FLAGS   = $(shell pkg-config --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf SDL2_gfx)
endif

all: $(OBJ_NAME)

$(OBJ_NAME): $(SRCS) | bin
	$(CC) $(SRCS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

bin:
	mkdir -p bin

.PHONY: all