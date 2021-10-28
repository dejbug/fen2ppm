@ECHO off

ECHO -- [ ENTER ] appveyor.bat

IF NOT EXIST appveyor.yml (
	ECHO -- [ ERROR ] this should only be called from root dir.
	GOTO :EOF
)

:setup
:setup-mingw
SET PATH=C:\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0;%PATH%
:setup-upx
IF NOT EXIST upx-3.96-win32.zip curl -kLO https://github.com/upx/upx/releases/download/v3.96/upx-3.96-win32.zip
7z x upx-3.96-win32.zip
SET PATH=%APPVEYOR_BUILD_FOLDER%\upx-3.96-win32;%PATH%

:info
mingw32-make --version | head -1
g++ --version | head -1
7z --help | head -3

:make
mingw32-make -j1 --keep-going

:deploy
CD deploy
COPY ..\README.md .
COPY ..\LICENSE .
7z a ..\%APPVEYOR_PROJECT_NAME%-%APPVEYOR_BUILD_NUMBER%.7z .

ECHO -- [ LEAVE ] appveyor.bat
