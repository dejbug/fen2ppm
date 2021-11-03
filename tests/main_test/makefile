SHELL := cmd.exe
ROOT := ../..

BINARY ?= $(ROOT)/deploy/fen2ppm.exe
FONTS ?= $(ROOT)/fonts

_FONT := -f $(FONTS)/Alpha.ttf
_MAP := -m "opjhnbtrwqlk"

# CMD := deploy\fen2ppm.exe -o "" -f "" -m "" -s 0 -g 0 -c ""

RANGE := 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15

.PHONY : all
all : $(foreach i,$(RANGE),fen$(i).ppm)

fen1.ppm : $(BINARY) ; -$< -o ""
fen2.ppm : $(BINARY) ; -$< -o $@ -f ""
fen3.ppm : $(BINARY) ; -$< -o $@ -f 0
fen4.ppm : $(BINARY) ; -$< -o $@ $(_FONT) -m ""
fen5.ppm : $(BINARY) ; -$< -o $@ $(_FONT) -m 0
fen6.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s ""
fen7.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 0
fen8.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64
fen9.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g ""
fen10.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 3
fen11.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 0
fen12.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 0 -c ""
fen13.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 0 -c 0
fen14.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 0 -c 123
fen15.ppm : $(BINARY) ; -$< -o $@ $(_FONT) $(_MAP) -s 64 -g 0 -c 333,ff0033

$(BINARY) : makefile ; $(MAKE) -C $(ROOT)

.PHONY : clean run
clean : ; DEL *.ppm out.txt *.fot 2>NUL
run : test.exe ; @.\$<
