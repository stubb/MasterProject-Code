ifeq ($(OS),Windows_NT)
	CXX = gcc
	SDL_LIB = -LC:/SDL2/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL2_net
	SDL_INCLUDE = -IC:/SDL2/x86_64-w64-mingw32/include
	LDFLAGS = -lmingw32 -lws2_32 -mwindows -mconsole $(SDL_LIB)
	EXE = renderingClient.exe
else
	ifeq ($(shell uname),Linux)
		CXX = gcc
		SDL_LIB = -L/usr/lib -lSDL2 -Wl,-rpath=/usr/lib -lSDL2_net
		SDL_INCLUDE = -I/usr/include
		CXXFLAGS = -Wall -c -std=c99
		LDFLAGS = $(SDL_LIB)
		EXE = RenderingClient.app
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang
		SDL = -framework SDL2 -framework SDL2_image
		CXXFLAGS = -Wall -c -std=c99
		LDFLAGS = $(SDL)
		EXE = renderingClient
	endif
endif

CXXFLAGS = -Wall -c -std=c99 $(SDL_INCLUDE)

all: $(EXE)

$(EXE): renderingClient.o
	$(CXX) $< $(LDFLAGS) -o $@

renderingClient.o: rc.c
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
