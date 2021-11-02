SHELL := cmd.exe

CXXFLAGS := -Ic:\dev\googletest-release-1.11.0\include
CXXFLAGS += -Lc:\dev\googletest-release-1.11.0\lib

LDFLAGS := -lgtest
# LDFLAGS += -lgtest_main
LDFLAGS += -lgdi32

test.exe : test.cpp ../*.h ; $(CXX) -o $@ $< $(CXXFLAGS) $(LDFLAGS)

.PHONY : clean run
clean : ; DEL test.exe 2>NUL
run : test.exe ; @.\$<
