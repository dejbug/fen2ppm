@ECHO OFF
REM Run this from anywhere like "c:\fen2ppm> fonts/run.bat Merida".

SET JUMP=
SET FONT=%~dp0

SET MAP="opmnvbtrwqlk"
SET ARG_O=fen.ppm
SET ARG_S=128
SET ARG_C=e8fdb7/e3e3e3/5ba2e0/ed60ac/000/000
SET ARG_F=rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR
SET ARG_G=0

IF "%1"=="" (GOTO Usage) ELSE (SET JUMP=%1)
IF "%2"=="" (GOTO Jump) ELSE (SET ARG_S=%2)
IF "%3"=="" (GOTO Jump) ELSE (SET ARG_C=%3)
IF "%4"=="" (GOTO Jump) ELSE (SET ARG_F=%4)
:Jump
GOTO %JUMP%

:Usage
ECHO Usage: run.bat FACE [SQUARE-SIZE [COLORS [FEN]]]
ECHO FACE: Alpha, Berlin, Kingdom, Leipzig, Merida.
GOTO :EOF

:Run
SET CMD=deploy\fen2ppm.exe -f %FONT% -m %MAP% -o %ARG_O% -s %ARG_S% -c %ARG_C% -g %ARG_G% %ARG_F%
ECHO %CMD%
%CMD%
GOTO :EOF

:Alpha
SET FONT=%FONT%%1.ttf
SET MAP="opjhnbtrwqlk"
GOTO :Run

:Berlin
SET FONT=%FONT%%1.ttf
SET MAP="opjhnbtrwqlk"
GOTO :Run

:Kingdom
SET FONT=%FONT%%1.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run

:Leipzig
SET FONT=%FONT%%1.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run

:Merida
SET FONT=%FONT%%1.TTF
SET MAP="opmnvbtrwqlk"
GOTO :Run
