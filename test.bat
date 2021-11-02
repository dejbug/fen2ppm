@ECHO OFF

IF "%1"=="run" GOTO Main
IF "%1"=="clean" GOTO Main
GOTO Usage

:Main
make -C app\theme_test %1
make -C lib\argument_parser_test %1
make -C lib\shlex_test %1
goto :EOF

:Usage
ECHO Usage:
ECHO   %0 run
ECHO   %0 clean
GOTO :EOF
