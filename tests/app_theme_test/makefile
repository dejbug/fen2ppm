SHELL := cmd.exe
ROOT := ../..

SOURCE ?= $(ROOT)/app/theme_t.h
LIBS ?= gdi32

CXXFLAGS := -Wall -Wextra -pedantic
CXXFLAGS += -Ic:\dev\googletest-release-1.11.0\include
CXXFLAGS += -Lc:\dev\googletest-release-1.11.0\lib
CXXFLAGS += -I$(ROOT)

LDFLAGS := -lgtest
# LDFLAGS += -lgtest_main
LDFLAGS += $(LIBS:%=-l%)

test.exe : test.cpp $(SOURCE) ; $(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

.PHONY : clean run
clean : ; DEL test.exe *.o 2>NUL
run : test.exe ; @.\$<
