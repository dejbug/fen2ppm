SHELL := cmd.exe
NAME := fen2ppm

CXX := g++
CXXFLAGS :=
CXXFLAGS += -Wall -Wextra -pedantic
CXXFLAGS += -std=c++17 -fconcepts
CXXFLAGS += -Wno-unused-parameter
# CXXFLAGS += -Wl,--subsystem=windows
# CXXFLAGS += -lgdi32 -lshell32

LDFLAGS := -lgdi32

deploy/$(NAME).exe : build/main.o | deploy
build/main.o : main.cpp | build

deploy : ; IF NOT EXIST $@ MKDIR $@
build : ; IF NOT EXIST $@ MKDIR $@

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll, $^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp %.c, $^) $(CXXFLAGS)

.PHONY : clean reset run
clean : ; IF EXIST build RMDIR /Q /S build
reset : | clean ; IF EXIST deploy RMDIR /Q /S deploy
run : deploy/$(NAME).exe ; @$<

.PHONY : release
release : deploy/$(NAME).exe
	COPY LICENSE deploy
	COPY README.md deploy
	upx -9 deploy/*.exe
