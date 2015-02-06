ifeq ($(OS),Windows_NT)
	CXX = g++
	OPENCV_LIB = -LC:/OpenCV/x64/mingw/lib -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
	OPENCV_INCLUDE = -IC:/OpenCV/include
	SDL_LIB = -LC:/SDL2/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL_net
	SDL_INCLUDE = -IC:/SDL2/x86_64-w64-mingw32/include
	LDFLAGS = -Wl,--stack,1000000000 -lmingw32 -lws2_32 -mwindows -mconsole $(SDL_LIB) $(OPENCV_LIB)
	EXE = ApplicationServer.exe
else
	ifeq ($(shell uname),Linux)
		CXX = g++
		SDL_LIB = -L/usr/lib -lSDL2 -lSDL_net -Wl,-rpath=/usr/lib
		SDL_INCLUDE = -I/usr/include
		OPENCV_INCLUDE =`pkg-config --cflags opencv`
		OPENCV_LIB =`pkg-config --libs opencv`
		LDFLAGS = $(SDL_LIB) $(OPENCV_LIB)
		EXE = ApplicationServer.app
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang
		SDL = -framework SDL2 -framework SDL_net
		LDFLAGS = $(SDL)
		EXE = ApplicationServer
	endif
endif

CXXFLAGS = -Wall -c -std=c++11 $(SDL_INCLUDE) $(OPENCV_INCLUDE)

all: $(EXE)

$(EXE): ApplicationServer.o
	$(CXX) $< $(LDFLAGS) -o $@

ApplicationServer.o: as.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
