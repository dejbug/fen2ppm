SHELL := cmd.exe
NAME := fen2ppm
TEST_1 := -f MERIFONTNEW.TTF -t "opmnvbtrwqlk" -o fen.ppm -s 128 -c ff0000/0000ff/ffaaaa/aaaaff "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
TEST_2 := -f MERIFONTNEW.TTF -t "opmnvbtrwqlk" -s 16 -c ff0000/0000ff/ffaaaa/aaaaff "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
TEST_3 := -f MERIFONTNEW.TTF -t "opmnvbtrwqlk" -o fen.ppm -s 128 -x "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
TEST_4 := -f MERIFONTNEW.TTF -t "opmnvbtrwqlk" -o fen.ppm -s

CXX := g++
CXXFLAGS :=
CXXFLAGS += -Wall -Wextra -pedantic
CXXFLAGS += -Wno-unused-parameter
CXXFLAGS += -Wl,--subsystem=console

LDFLAGS := -lgdi32

deploy/$(NAME).exe : build/main.o | deploy
build/main.o : main.cpp *.h lib/*.h lib/*/*.h app/*.h | build

deploy : ; IF NOT EXIST $@ MKDIR $@
build : ; IF NOT EXIST $@ MKDIR $@

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll, $^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp %.c, $^) $(CXXFLAGS)

.PHONY : clean reset run test
clean : ; IF EXIST build RMDIR /Q /S build && DEL *.ppm 2>NUL
reset : | clean ; IF EXIST deploy RMDIR /Q /S deploy
run : deploy/$(NAME).exe ; @$< $(TEST_1)

.PHONY : release
release : deploy/$(NAME).exe
	COPY LICENSE deploy
	COPY README.md deploy
	upx -9 deploy/*.exe
