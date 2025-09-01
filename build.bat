@echo off
echo ================================
echo Building Keyboard Macro Tool v1.2
echo ================================
echo.

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

REM Compile the program
echo Compiling macro_tool.cpp...
g++ -o macro_tool.exe macro_tool.cpp -luser32 -std=c++17 -O2 -static

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
echo Executable created: macro_tool.exe
echo.
echo To run the macro tool:
echo   macro_tool.exe           (uses macro.ini)
echo   macro_tool.exe config.ini (uses custom config file)
echo.
pause
