C_FLAGS = -std=c++17 `sdl2-config --cflags` -I src/include

ifeq ($(OS),Windows_NT)
LIBS = -Wl,-subsystem,windows `sdl2-config --libs` -lglew32 -lglu32 -lopengl32
CPP = $(shell find ./src -name *.cpp)
SHADERS = $(shell find ./src/shaders/*)
else
LIBS = `sdl2-config --libs` -lGLEW -lGLU -lGL
CPP = $(shell find ./src -name *.cpp)
SHADERS = $(shell find ./src/shaders/*)
endif

OBJ = $(patsubst ./src/%.cpp, ./build/%.o, $(CPP))

EXE_NAME = raymarcher

all: x64
	cp -r ./src/shaders bin
	cp -r ./dlls/* bin

rebuild-run: clean run

run: all
	./bin/$(EXE_NAME)-x64

$(OBJ): ./build/%.o : ./src/%.cpp
	mkdir -p $(dir $@) && \
	g++ -c -o $@ -Og -g $(patsubst build/%.o, src/%.cpp, $@) $(LIBS) $(C_FLAGS)

.PHONY: x64

x64: $(OBJ)
	mkdir -p ./bin && \
	g++ -o ./bin/$(EXE_NAME)-x64 $^ $(LIBS)
	chmod a+x ./bin/$(EXE_NAME)-x64

.PHONY: clean
clean:
	rm -rf ./build
	rm -rf ./bin
