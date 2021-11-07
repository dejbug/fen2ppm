@ECHO OFF

SET BIN="%~dp1deploy\fen2ppm.exe"
SET OUT=-o fen.ppm
SET FONT=
SET MAP=
SET COLORS=
SET SIZE=-s 64
SET GAP=-g 0
SET FEN=rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR

IF "%1"=="" GOTO Usage
IF "%1"=="run" GOTO Make
IF "%1"=="clean" GOTO Make
GOTO Test%1

:Test1
REM cls && make && deploy\fen2ppm.exe -o fen.ppm && fen.ppm
GOTO AddArgs

:Test2
REM cls && make && deploy\fen2ppm.exe -o fen.ppm -c ccc/a00 && fen.ppm
SET COLORS=-c ccc/a00
GOTO AddArgs

:Test3
REM cls && make && deploy\fen2ppm.exe -o fen.ppm -c 00a/0f0 -s 128 -g 20 && fen.ppm
SET COLORS=-c 00a/0f0
SET SIZE=-s 128
SET GAP=-g 20
GOTO AddArgs

:Test4
REM cls && make && deploy\fen2ppm.exe -o fen.ppm -s 128 8/8/8/8/8/8/8/3QK3 && fen.ppm
SET SIZE=-s 128
SET FEN=8/8/8/8/8/8/8/3QK3
GOTO AddArgs

:TestAlpha
:TestBerlin
:TestKingdom
:TestLeipzig
:TestMerida
IF NOT EXIST "fonts\%1.ttf" (
	ECHO File "%~dp1fonts\%~1.ttf" does not exist.
	GOTO :EOF
)
SET FONT=-f "%~dp1fonts\%~1.ttf"
IF "%1"=="Alpha" SET MAP=-m 4
IF "%1"=="Berlin" SET MAP=-m 4
IF "%1"=="Kingdom" SET MAP=-m 3
IF "%1"=="Leipzig" SET MAP=-m 3
IF "%1"=="Merida" SET MAP=-m 3
GOTO AddArgs

:AddArgs
IF "%2"=="" (GOTO Run) ELSE (SET MAP=-m %2)
IF "%3"=="" (GOTO Run) ELSE (SET COLORS=-c %3)
GOTO Run

:Run
cls
make
SET CMD=%BIN% %OUT% %FONT% %MAP% %COLORS% %SIZE% %GAP% %FEN%
ECHO Running: %CMD%
%CMD%
fen.ppm
GOTO :EOF

:Make
make -C tests\app_theme_test %1
make -C tests\lib_argument_parser_test %1
make -C tests\lib_shlex_test %1
make -C tests\lib_test %1
GOTO :EOF

:Usage
ECHO Usage:
ECHO   %0 run^|clean
ECHO   %0 1^|2^|3^|...
ECHO   %0 Alpha^|Berlin^|Kingdom^|Leipzig^|Merida [MAP [COLORS]]
ECHO Notes:
ECHO   Alpha and Berlin use internal MAP 4 by default.
ECHO   Kingdom, Leipzig, and Merida use internal MAP 3 by default.
ECHO   Colors can be a string of up to six slash-separated (no spaces!) RGB hexes.
ECHO   Tests 1-4 work in newspaper mode which uses only the first two colors.
ECHO   The font tests work in themed mode and use up to six colors.
ECHO Warning: Font names are case-sensitive!
GOTO :EOF
