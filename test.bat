@ECHO OFF

IF "%1"=="" GOTO Usage
IF "%1"=="run" GOTO Make
IF "%1"=="clean" GOTO Make
GOTO Test%1

:Test1
cls && make && deploy\fen2ppm.exe -o fen.ppm && fen.ppm
GOTO :EOF

:Test2
cls && make && deploy\fen2ppm.exe -o fen.ppm -c eee/fee/0ef/f00 && fen.ppm
GOTO :EOF

:Test3
cls && make && deploy\fen2ppm.exe -o fen.ppm -c 00a/0f0/f00/fff/afa -s 128 -g 20 && fen.ppm
GOTO :EOF

:Test4
cls && make && deploy\fen2ppm.exe -o fen.ppm -s 128 8/8/8/8/8/8/8/3QK3 && fen.ppm
GOTO :EOF

:Make
make -C tests\app_theme_test %1
make -C tests\lib_argument_parser_test %1
make -C tests\lib_shlex_test %1
make -C tests\lib_test %1
GOTO :EOF

:Usage
ECHO Usage:
ECHO   %0 run
ECHO   %0 clean
ECHO   %0 1
ECHO   %0 2
ECHO   %0 ...
GOTO :EOF
