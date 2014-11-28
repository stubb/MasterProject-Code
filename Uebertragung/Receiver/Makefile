ifeq ($(OS),Windows_NT)
	CXX = g++
	LDFLAGS = -lmingw32 -mwindows -mconsole
	DEPS = 
	EXE = Client.exe
else
	ifeq ($(shell uname),Linux)
		CXX = clang++
		CXXFLAGS = $(shell pkg-config gtk+-3.0 --cflags) -c -g -std=c++11 \
			-fstack-protector \
			--param=ssp-buffer-size=4 \
			-pthread \
			-fno-exceptions \
			-fno-strict-aliasing \
			-Wno-unused-parameter \
			-Wno-missing-field-initializers \
			-fvisibility=hidden \
			-pipe \
			-fPIC \
			-pthread \
			-I/usr/include/glib-2.0 \
			-I/usr/lib/x86_64-linux-gnu/glib-2.0/include \
			-I/usr/include/gtk-2.0 \
			-I/usr/lib/x86_64-linux-gnu/gtk-2.0/include \
			-I/usr/include/atk-1.0 \
			-I/usr/include/cairo \
			-I/usr/include/gdk-pixbuf-2.0 \
			-I/usr/include/pango-1.0 \
			-I/usr/include/gio-unix-2.0/ \
			-I/usr/include/pixman-1 \
			-I/usr/include/freetype2 \
			-I/usr/include/libpng12 \
			-I/usr/include/harfbuzz \
			-I/usr/include/nss \
			-I/usr/include/nspr \
			-Wno-format \
			-Wno-unused-result \
			-O2 \
			-fno-ident \
			-fdata-sections \
			-ffunction-sections \
			-funwind-tables \
			-fno-rtti \
			-fno-threadsafe-statics \
			-fvisibility-inlines-hidden \
			-Wno-deprecated \
			-DFOO \
			-DWEBRTC_POSIX
		LDFLAGS = $(shell pkg-config gtk+-3.0 --libs) -lgthread-2.0 \
	-lgtk-x11-2.0 \
	-lgdk-x11-2.0 \
	-latk-1.0 \
	-lgio-2.0 \
	-lpangoft2-1.0 \
	-lpangocairo-1.0 \
	-lgdk_pixbuf-2.0 \
	-lcairo \
	-lpango-1.0 \
	-lfreetype \
	-lfontconfig \
	-lgobject-2.0 \
	-lglib-2.0 \
	-lX11 \
	-lXcomposite \
	-lXext \
	-lXrender \
	-lnss3 \
	-lnssutil3 \
	-lsmime3 \
	-lplds4 \
	-lplc4 \
	-lnspr4 \
	-ldl \
	-lcrypto \
	-lrt \
	-lexpat \
	-lm \
	-lWebRTC \
	-lpthread
		DEPS = 
		EXE = Client
	endif

	ifeq ($(shell uname), Darwin)
		CXX = clang
		CXXFLAGS = -Wall -c
		DEPS =
		EXE = Client
	endif
endif

all: $(EXE)

$(EXE): main.o conductor.o defaults.o main_wnd.o per_connection_client.o linux/main_wnd.o 
	$(CXX) $< $(LDFLAGS) -o $@

main.o: main.cc
	$(CXX) $(CXXFLAGS) $< -o $@

conductor.o: conductor.cc
	$(CXX) $(CXXFLAGS) $< -o $@

defaults.o: defaults.cc
	$(CXX) $(CXXFLAGS) $< -o $@

#flagdefs.o: flagdefs.cc
#	$(CXX) $(CXXFLAGS) $< -o $@

main_wnd.o: main_wnd.cc
	$(CXX) $(CXXFLAGS) $< -o $@

linux/main_wnd.o: linux/main_wnd.cc
	$(CXX) $(CXXFLAGS) $< -o $@

per_connection_client.o: per_connection_client.cc
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
ifeq ($(OS),Windows_NT)
	del *.o && del $(EXE)
else
	rm *.o && rm $(EXE)
endif
