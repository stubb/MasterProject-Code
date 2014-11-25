ifeq ($(OS),Windows_NT)
	CXX = g++
	LDFLAGS = -lmingw32 -mwindows -mconsole
	EXE = Client.exe
else
	ifeq ($(shell uname),Linux)
		CXX = g++
		CXXFLAGS = -Wall -c -std=c99
		EXE = Client
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang
		CXXFLAGS = -Wall -c -std=c99
		EXE = Client
	endif
endif

CXXFLAGS = -Wall

all: $(EXE)

$(EXE): main.o
	$(CXX) $< $(LDFLAGS) -o $@

main.o: main.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
