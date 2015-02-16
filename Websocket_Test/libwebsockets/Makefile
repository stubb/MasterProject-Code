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
		LIBWEBSOCKETS_INCLUDE = -I/usr/include
		LIBWEBSOCKETS_LIB = -lwebsockets
		LDFLAGS = $(LIBWEBSOCKETS_LIB)
		EXE = Server.app
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
