SHELL := cmd.exe
RC := windres

TEST_1 := -o fen.ppm -s 64 -c ff0000/0000ff/ffaaaa/aaaaff "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

NAME := fen2ppm

CXX := g++
CXXFLAGS :=
CXXFLAGS += -Wall -Wextra -pedantic
CXXFLAGS += -Wno-unused-parameter
CXXFLAGS += -Wl,--subsystem=console

LDFLAGS := -lgdi32

deploy/$(NAME).exe : build/main.o build/resource.res.o | deploy
build/main.o : main.cpp lib/*.h app/*.h | build
build/resource.res.o : res/resource.rc res/*.ttf

deploy : ; IF NOT EXIST $@ MKDIR $@
build : ; IF NOT EXIST $@ MKDIR $@

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll %res, $^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp %.c, $^) $(CXXFLAGS)
%.res.o : ; $(RC) $< $@

.PHONY : clean reset run test
clean : ; IF EXIST build RMDIR /Q /S build && DEL *.ppm *.fot 2>NUL
reset : | clean ; IF EXIST deploy RMDIR /Q /S deploy
run : deploy/$(NAME).exe ; @$< $(TEST_1)

.PHONY : release
release : deploy/$(NAME).exe
	COPY LICENSE deploy
	COPY README.md deploy
	upx -9 deploy/*.exe
