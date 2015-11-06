ifeq ($(OS),Windows_NT)
	CXX = gcc
	SDL_LIB = -LC:/SDL2/x86_64-w64-mingw32/lib -lSDL2main -lSDL2 -lSDL2_net
	SDL_INCLUDE = -IC:/SDL2/x86_64-w64-mingw32/include
	LDFLAGS = -lmingw32 -lws2_32 -mwindows -mconsole $(SDL_LIB)
	EXE = renderingClient.exe
else
	ifeq ($(shell uname),Linux)
		CXX = gcc
		SDL_LIB = -lSDL2_net -lSDL2
		SDL_INCLUDE = -I/usr/include
		CXXFLAGS = -Wall -c -std=c99
		LDFLAGS = -L/usr/lib -Wl,-rpath=libs:/usr/lib $(SDL_LIB)
		EXE = RenderingClient.app
	endif

	ifeq ($(shell uname), Darwin)
		FRAMEWORK_PATH=./bin/mac64/libs/
		APP_NAME=RenderingClient
		CXX = clang
		SDL_INCLUDE = -F $(FRAMEWORK_PATH)
		SDL = -framework SDL2 -framework SDL2_net
		CXXFLAGS = -Wall -c -std=c99
		LDFLAGS = $(SDL)
		EXE = RenderingClient.out
	endif
endif

CXXFLAGS = -Wall -c -std=c99 $(SDL_INCLUDE)

Wall: $(EXE)

$(EXE): renderingClient.o
	$(CXX) $< $(LDFLAGS) -o $@

renderingClient.o: rc.c
	$(CXX) $(CXXFLAGS) $< -o $@

mac_clean_app:
	rm -rf "./build/$(APP_NAME).app/"
	
mac_package_app:
	mkdir -p "./build/$(APP_NAME).app"/Contents/{MacOS,Resources}
	cp -R "$(FRAMEWORK_PATH)/SDL2.framework" "./build/$(APP_NAME).app/Contents/Resources/"
	cp -R "$(FRAMEWORK_PATH)/SDL2_net.framework" "./build/$(APP_NAME).app/Contents/Resources/"
	cp Info.plist "./build/$(APP_NAME).app/Contents/"
	sed -e "s/APP_NAME/$(APP_NAME)/g" -i "" "./build/$(APP_NAME).app/Contents/Info.plist"
	cp $(EXE) "./build/$(APP_NAME).app/Contents/MacOS/$(APP_NAME)"

.PHONY: clean
clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif