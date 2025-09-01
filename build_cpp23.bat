@echo off
echo ================================
echo Building Keyboard Macro Tool v2.0 (C++23)
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

REM Check g++ version for C++23 support
echo Checking compiler version...
g++ --version | findstr /R "1[3-9]\." >nul 2>&1
if %errorlevel% neq 0 (
    echo WARNING: Your g++ version might not fully support C++23.
    echo C++23 requires GCC 13+ for full feature support.
    echo Some features may not be available.
    echo.
)

REM Compile the C++23 version
echo Compiling macro_tool_cpp23.cpp with C++23 features...
g++ -o macro_tool_v2.exe macro_tool_cpp23.cpp -luser32 -std=c++23 -O3 -Wall -Wextra -pedantic -static

if %errorlevel% neq 0 (
    echo.
    echo ERROR: C++23 compilation failed!
    echo Trying fallback to C++20...
    g++ -o macro_tool_v2.exe macro_tool_cpp23.cpp -luser32 -std=c++20 -O3 -Wall -static
    
    if %errorlevel% neq 0 (
        echo.
        echo Falling back to original version...
        g++ -o macro_tool.exe macro_tool.cpp -luser32 -std=c++23 -O2 -static
        
        if %errorlevel% neq 0 (
            echo.
            echo ERROR: All compilation attempts failed!
            pause
            exit /b 1
        )
        
        echo.
        echo Built original version as macro_tool.exe
    ) else (
        echo.
        echo Built with C++20 features as macro_tool_v2.exe
    )
) else (
    echo.
    echo Successfully built C++23 version as macro_tool_v2.exe
)

echo.
echo ================================
echo Build completed!
echo ================================
echo.
echo Available executables:
if exist macro_tool.exe echo   macro_tool.exe     - Original version
if exist macro_tool_v2.exe echo   macro_tool_v2.exe  - C++23 enhanced version
echo.
echo To run the macro tool:
echo   macro_tool.exe macro.ini       (original)
echo   macro_tool_v2.exe macro.ini    (C++23 version)
echo.
pause
