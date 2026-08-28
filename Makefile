OBJS = src\main.cpp src\Game.cpp src\FrameManager.cpp src\TextureManager.cpp src\Map.cpp src\Vector2D.cpp src\Collision.cpp src\EntityComponentSystem\EntityComponentSystem.cpp

CC = g++

INCLUDE_PATHS = -I.\src -IC:/msys64/ucrt64/include/SDL2

LIBRARY_PATHS = -LC:/msys64/ucrt64/lib

COMPILER_FLAGS = -std=c++20 -static-libgcc -static-libstdc++

LINKER_FLAGS = -static -lmingw32 -lSDL2main $(shell pkg-config --static --libs sdl2 SDL2_image SDL2_mixer SDL2_ttf SDL2_gfx)

OBJ_NAME = bin\Bellicus.exe

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)