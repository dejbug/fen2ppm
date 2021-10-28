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

$(NAME).exe : main.o
main.o : main.cpp

%.exe : ; $(CXX) -o $@ $(filter %.o %.a %.dll, $^) $(CXXFLAGS) $(LDFLAGS)
%.o : ; $(CXX) -o $@ -c $(filter %.cpp %.c, $^) $(CXXFLAGS)

.PHONY : clean reset run
clean : ; DEL *.o 2>NUL
reset : | clean ; DEL $(NAME).exe *.ppm 2>NUL
run : $(NAME).exe ; @.\$<
