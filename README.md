[![Build status](https://ci.appveyor.com/api/projects/status/rw6f5q1441rw3bl7/branch/main?svg=true)](https://ci.appveyor.com/project/dejbug/fen2ppm/branch/main)


# fen2ppm

Renders [FEN](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation) (chess positions) to [PPM](https://en.wikipedia.org/wiki/Netpbm) (images).

# Usage

Currently the functionality is very limited. You can render only a test image. You are able to set the square size (`-s`), the output file's name (`fen.ppm`), the FEN text, and the chess font's path. For example, typing:

```
fen2ppm.exe -s 128 "fen.ppm" "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "C:\Windows\Fonts\MERIFONTNEW.TTF"
```

will create this image. (This one is a PNG but the output will be in PPM format.)

![demo](../assets/fen.png?raw=true)

**Note** that different fonts assign their glyphs to (slightly) different codepoints. We will have to pass a translation table as an argument too. You can still try other fonts but they probably won't all work.

# Download

Look in the releases tab for stable builds. Look [here](https://ci.appveyor.com/project/dejbug/fen2ppm/branch/main/artifacts) for works in progress.
