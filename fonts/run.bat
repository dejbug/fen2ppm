@ECHO OFF
REM Run this from anywhere like "c:\fen2ppm> fonts/run.bat Merida".

SET JUMP=
SET FONT=%~dp0

SET MAP="opmnvbtrwqlk"
SET ARG_S=128
SET ARG_C=fdb7b7/d1d1d1/5aa0dc/dc5aa0
SET ARG_O=fen.ppm

IF "%1"=="" (GOTO Usage) ELSE (SET JUMP=%1)
IF "%2"=="" (GOTO Jump) ELSE (SET ARG_S=%2)
IF "%3"=="" (GOTO Jump) ELSE (SET ARG_C=%3)
IF "%4"=="" (GOTO Jump) ELSE (SET ARG_O=%4)
:Jump
GOTO %JUMP%

:Usage
ECHO Usage: run.bat FACE [SQUARE-SIZE [COLORS [OUT-PATH]]]
ECHO FACE: Alpha, Berlin, Kingdom, Leipzig, Merida.
GOTO :EOF

:Run
SET CMD=deploy\fen2ppm.exe -f %FONT% -m %MAP% -o %ARG_O% -s %ARG_S% -c %ARG_C% -g 0 "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR"
ECHO %CMD%
%CMD%
GOTO :EOF

:Alpha
SET FONT=%FONT%Alpha.ttf
SET MAP="opjhnbtrwqlk"
GOTO :Run

:Berlin
SET FONT=%FONT%Berlin.ttf
SET MAP="opjhnbtrwqlk"
GOTO :Run

:Kingdom
SET FONT=%FONT%KINGFONT.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run

:Leipzig
SET FONT=%FONT%LEIPFONT.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run

:Merida
SET FONT=%FONT%MERIFONTNEW.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run
