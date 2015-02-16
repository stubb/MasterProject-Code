SOURCES = test_server.cpp
OBJECTS=$(SOURCES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	CXX = g++
	LIBWEBSOCKETS_LIB = -L./ -lwebsockets
	LIBWEBSOCKETS_INCLUDE = -IC:\Users\kili\libwebsockets\lib
	LDFLAGS = -mwindows -mconsole $(LIBWEBSOCKETS_LIB)
	EXE = Server.exe
else
	ifeq ($(shell uname),Linux)
		CXX = g++
		SDL_LIB = -L/usr/lib -lSDL2 -lSDL2_net -Wl,-rpath=/usr/lib
		SDL_INCLUDE = -I/usr/include
		OPENCV_INCLUDE =`pkg-config --cflags opencv`
		OPENCV_LIB =`pkg-config --libs opencv`
		WEBSOCKET_INCLUDE =-I/usr/include
		WEBSOCKET_LIB =-lboost_system
		LDFLAGS = $(WEBSOCKET_LIB) $(SDL_LIB) $(OPENCV_LIB) 
		EXE =ApplicationServer.app
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang
		SDL = -framework SDL2 -framework SDL2_net
		LDFLAGS = $(SDL)
		EXE = ApplicationServer
	endif
endif

CXXFLAGS = -Wall -c -std=c++11 $(LIBWEBSOCKETS_INCLUDE)

all: $(SOURCES) $(EXE)

$(EXE): $(OBJECTS) 
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
	
.cpp.o:
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
