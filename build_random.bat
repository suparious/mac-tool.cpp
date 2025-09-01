@echo off

REM Path to the g++ compiler
SET PATH=%USERPROFILE%\repos\mingw64\bin;%PATH%

REM Path to the repository and binary output directory
SET REPO=%USERPROFILE%\repos\mac-tool.cpp
SET BIN=%USERPROFILE%\bin

REM Check if g++ is available
where g++ >nul 2>nul
if %errorlevel% neq 0 (
    echo ERROR: g++ compiler not found!
    echo Please install MinGW-w64 or another C++ compiler.
    echo.
    echo You can download MinGW-w64 from:
    echo https://www.mingw-w64.org/downloads/
    echo.
    echo After installation, make sure g++ is in your PATH.
    pause
    exit /b 1
)

REM Generate a random string for the binary name
setlocal enabledelayedexpansion
set "chars=ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
set "random_string="
for /L %%i in (1,1,16) do (
    set /a "rand_index=!RANDOM! %% 62"
    for %%j in (!rand_index!) do set "random_string=!random_string!!chars:~%%j,1!"
)
echo Random string generated: !random_string!

REM Create the binary output directory if it doesn't exist
if not exist %BIN% (
    mkdir %BIN%
)

REM Pull the latest changes from the repository
cd %REPO%
git pull

REM Compile the program
echo Compiling macro_tool.cpp to %BIN%\%random_string%.exe...
g++ -o %BIN%\%random_string%.exe macro_tool.cpp -luser32 -std=c++17 -O2 -static

if %errorlevel% neq 0 (
    echo.
    echo ERROR: Compilation failed!
    echo Please check the error messages above.
    pause
    exit /b 1
)

echo.
echo ================================
echo Build successful!
echo ================================
echo.
echo Executable created: %BIN%\%random_string%.exe
echo.
echo To run the macro tool:
echo   %BIN%\%random_string%.exe           (uses macro.ini)
echo   %BIN%\%random_string%.exe config.ini (uses custom config file)
echo.
