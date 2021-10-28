SHELL := cmd.exe
NAME := fen2ppm
TEST := -s 128 fen.ppm "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" MERIFONTNEW.TTF

CXX := g++
CXXFLAGS :=
CXXFLAGS += -Wall -Wextra -pedantic
CXXFLAGS += -std=c++17 -fconcepts
CXXFLAGS += -Wno-unused-parameter
# CXXFLAGS += -Wl,--subsystem=windows

LDFLAGS := -lgdi32

deploy/$(NAME).exe : build/main.o | deploy
build/main.o : main.cpp  *.h | build

deploy : ; IF NOT EXIST $@ MKDIR $@
build : ; IF NOT EXIST $@ MKDIR $@

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll, $^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp %.c, $^) $(CXXFLAGS)

.PHONY : clean reset run test
clean : ; IF EXIST build RMDIR /Q /S build && DEL *.ppm 2>NUL
reset : | clean ; IF EXIST deploy RMDIR /Q /S deploy
run : deploy/$(NAME).exe ; @$< $(TEST)

.PHONY : release
release : deploy/$(NAME).exe
	COPY LICENSE deploy
	COPY README.md deploy
	upx -9 deploy/*.exe
