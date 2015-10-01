SOURCES = as.cpp tinyxml2.cpp WebSocketServer.cpp NetworkHandler.cpp
OBJECTS=$(SOURCES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	CXX = g++
	OPENCV_LIB = -LC:/OpenCV/x64/mingw/lib -lopencv_core248 -lopencv_highgui248 -lopencv_imgproc248
	OPENCV_INCLUDE = -IC:/OpenCV/include
	SDL_LIB = -LC:/SDL2/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL2_net
	SDL_INCLUDE = -IC:/SDL2/x86_64-w64-mingw32/include
	LIBWEBSOCKET_LIB = -LC:/MinGW64/libwebsocket/lib -lwebsockets
	LIBWEBSOCKET_INCLUDE = -IC:/MinGW64/libwebsocket/include
	LDFLAGS = -lmingw32 -lws2_32 -mwindows -mconsole $(SDL_LIB) $(OPENCV_LIB) $(LIBWEBSOCKET_LIB)
	EXE = ApplicationServer.exe
else
	ifeq ($(shell uname),Linux)
		CXX = g++
		SDL_LIB = -L/usr/lib -lSDL2 -lSDL2_net
		SDL_INCLUDE = -I/usr/include
		OPENCV_INCLUDE =`pkg-config --cflags opencv`
		OPENCV_LIB =`pkg-config --libs opencv`
		LIBWEBSOCKET_INCLUDE =-I/usr/include
		LIBWEBSOCKET_LIB =-lwebsockets
		LDFLAGS = -Wl,-rpath=libs:/usr/lib $(LIBWEBSOCKET_LIB) $(SDL_LIB) $(OPENCV_LIB)
		EXE =ApplicationServer.app
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang++
		MACCES = -stdlib=libc++ -lstdc++
		SDL = -framework SDL2 -framework SDL2_net
		OPENCV_LIB = -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc  
		LIBWEBSOCKET = -L/usr/local/lib -lwebsockets
		LIBWEBSOCKET_INCLUDE = -I/usr/local/include/websockets
		LDFLAGS = $(SDL) $(OPENCV_LIB) $(LIBWEBSOCKET)
		EXE = ApplicationServer.app
	endif
endif

CXXFLAGS = -Wall -c -std=c++11 $(MACCES) $(SDL_INCLUDE) $(OPENCV_INCLUDE) $(LIBWEBSOCKET_INCLUDE)


all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS) 
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
