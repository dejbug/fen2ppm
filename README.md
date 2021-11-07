[![Build status](https://ci.appveyor.com/api/projects/status/rw6f5q1441rw3bl7/branch/main?svg=true)](https://ci.appveyor.com/project/dejbug/fen2ppm/branch/main)

# fen2ppm

Renders [FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) (chess positions) to [PPM](https://en.wikipedia.org/wiki/Netpbm) (images).

# Usage

A usage message is still missing while I'm figuring out the options. A preliminary set of possible commands is currently as follows:

The command line `fen2ppm.exe -o fen.ppm` Outputs a 512x512 board (64 pixel square size) to `fen.ppm` (if a file of that name already exists in your CWD it will be overwritten!). Since no font (`-f`) option was provided here it will use a default built-in font (the beautiful "[Chess Leipzig](http://www.enpassant.dk/chess/fonteng.htm)" font, courtesy of Armando Hernandez Marroquin). Because no colors (`-c`) option was given, it will draw in monochrome.

You can choose between themed or newspaper style output by giving either a 12 or 26 letter map. (Internal map 2 is 26 also, so try `-m 2`). This is because although there are only 12 distinct pieces (six types in two colors), for newspaper-style versions of each of these exist with a shaded background.

The full command line is this: `fen2ppm.exe -o OUTPATH -f FONT_PATH_OR_NAME -m FONT_MAPPING -c COLORS -s SQUARE_SIZE -g GAP`. OUTPATH is e.g. `"-o fen.ppm"`. SQUARE_SIZE is e.g. `-g 128`. GAP (between squares) is e.g. `-g 2`. FONT_PATH_OR_NAME is e.g. `-f "Chess Alpha"` or `-f C:\MyDownloadedFonts\ChessAlpha.ttf`.

FONT_MAPPING is e.g. `"ABCDEFGHIJKL"`. This is a twelve-letter string, each letter standing for one chess piece. It is necessary to give such a string because every font might have pieces' glyphs assigned to an arbitrary set of letters. How to read this string is using the following key: `"pPnNbBrRqQkK"`, each letter standing for a piece. So with the former mapping given on the command line, wherever a 'p' occurs in the FEN it will be translated to an 'A', whenever a 'P', it will become a 'B', and so on. There are currently 5 predefined maps which you can try with your font by giving `-m 0` through `-m 4`.

COLORS is e.g. `eee/fee/0ef/f00`, which would draw light grey and rose squares and light blue and red pieces. The format is DS/LS/DP/LP (D=dark, L=light, S=square, P=piece). You can give it three-digit, six-digit, or eight-digit hex numbers (with or without preceding '#'). The three- and six-digit hexes are known to you as HTML colors; (the eight-digit form includes an alpha channel, which is currently ignored).

A sample image.

![demo](../assets/fen.png?raw=true)

# Download

Look in the releases tab for versioned builds. Look [here](https://ci.appveyor.com/project/dejbug/fen2ppm/branch/main/artifacts) for works in progress (which will be deleted after a month).
