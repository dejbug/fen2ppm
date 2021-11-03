@ECHO OFF

IF "%1"=="run" GOTO Main
IF "%1"=="clean" GOTO Main
GOTO Usage

:Main
make -C tests\app_theme_test %1
make -C tests\lib_argument_parser_test %1
make -C tests\lib_shlex_test %1
goto :EOF

:Usage
ECHO Usage:
ECHO   %0 run
ECHO   %0 clean
GOTO :EOF
