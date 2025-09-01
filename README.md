# Keyboard Macro Tool for Windows

A lightweight, configurable keyboard macro automation tool for Windows. Perfect for automated testing and load testing of applications.

## Features

- **Simple Configuration**: Easy-to-read INI file format for defining macros
- **Precise Timing Control**: Specify exact key hold durations and pause intervals
- **Random Range Support**: Use ranges (e.g., 100-200) for more realistic, human-like input
- **Loop Support**: Repeat sequences of commands multiple times
- **Extensive Key Support**: Supports all standard keys, function keys, numpad, and special keys
- **Abort Function**: Press ESC at any time to stop macro execution
- **Command Line Support**: Specify custom config files via command line

## Requirements

- Windows OS (Windows 7 or later)
- C++ compiler (MinGW-w64 recommended)
- Administrator privileges may be required for some applications

## Compilation

### Method 1: Using the Build Script (Recommended)

Simply double-click `build.bat` or run it from the command line:

```bash
build.bat
```

### Method 2: Manual Compilation with MinGW-w64

If you have MinGW-w64 installed:

```bash
g++ -o macro_tool.exe macro_tool.cpp -luser32 -std=c++11 -O2 -static
```

### Method 3: Using Visual Studio Developer Command Prompt

```bash
cl macro_tool.cpp user32.lib /Fe:macro_tool.exe /O2
```

### Installing MinGW-w64 (if needed)

1. Download from: https://www.mingw-w64.org/downloads/
2. Recommended: Use MSYS2 installer from https://www.msys2.org/
3. After installation, run in MSYS2 terminal:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   ```
4. Add MinGW bin directory to your PATH (e.g., `C:\msys64\mingw64\bin`)

## Configuration

Edit `macro.ini` or create your own config file with the following commands:

### Available Commands

| Command | Syntax                         | Description                                             |
| ------- | ------------------------------ | ------------------------------------------------------- |
| PAUSE   | `pause <seconds>`              | Pause execution for specified seconds                   |
| PAUSE   | `pause <min>-<max>`            | Pause for random seconds within range                   |
| KEYDOWN | `keydown <key> <milliseconds>` | Press and hold key for specified milliseconds           |
| KEYDOWN | `keydown <key> <min>-<max>`    | Press and hold key for random milliseconds within range |
| KEY     | `key <key>`                    | Quick key press (50ms default)                          |
| LOOP    | `loop <count>`                 | Start a loop block that repeats count times             |
| ENDLOOP | `endloop`                      | End of loop block                                       |

### Supported Keys

**Letters**: A-Z  
**Numbers**: 0-9  
**Function Keys**: F1-F12  
**Special Keys**: SPACE, ENTER, TAB, ESCAPE, BACKSPACE, DELETE, INSERT  
**Arrow Keys**: UP, DOWN, LEFT, RIGHT  
**Modifier Keys**: SHIFT, CTRL, ALT, WIN  
**Navigation**: HOME, END, PAGEUP, PAGEDOWN  
**Numpad**: NUMPAD0-NUMPAD9, MULTIPLY, ADD, SUBTRACT, DECIMAL, DIVIDE  
**Symbols**: COMMA, PERIOD, SEMICOLON, QUOTE, SLASH, BACKSLASH, LEFTBRACKET, RIGHTBRACKET, MINUS, EQUALS, GRAVE

### Decimal Notation Support

You can use decimal notation for more precise timing:

```ini
pause 0.150    # Pause for 150 milliseconds
pause 1.5      # Pause for 1.5 seconds
pause 0.05-0.15  # Random pause between 50-150ms
```

### Example Configuration

```ini
# Simple typing test with fixed timing
pause 2
keydown H 100
keydown E 100
keydown L 100
keydown L 100
keydown O 100
key SPACE
keydown W 100
keydown O 100
keydown R 100
keydown L 100
keydown D 100
keydown ENTER 150

# Your custom macro with random ranges
pause 100
pause 87-130        # Random pause between 87-130 seconds
keydown Y 120-180   # Hold Y for random 120-180ms
pause 4-6           # Random pause between 4-6 seconds
keydown X 150-250   # Hold X for random 150-250ms

# Loop example for stress testing
loop 10
    keydown SPACE 100
    pause 0.5
endloop
```

## Usage

### Basic Usage

Run with default config file (macro.ini):

```bash
macro_tool.exe
```

### Custom Config File

Specify a different config file:

```bash
macro_tool.exe my_custom_macro.ini
```

### Human-like Testing

Use random ranges for more realistic input:

```bash
macro_tool.exe human_like.ini
```

### Execution Flow

1. Launch the tool
2. You have 3 seconds to switch to your target application
3. The macro begins executing
4. Press ESC at any time to abort

## Random Range Feature

### Syntax

You can specify ranges for both `pause` and `keydown` commands using the format `min-max`:

```ini
# Fixed timing
pause 5              # Always pause for 5 seconds
keydown A 100        # Always hold A for 100ms

# Random timing
pause 3-7            # Pause for random 3-7 seconds
keydown B 50-150     # Hold B for random 50-150ms
```

### Benefits

- **Avoid Detection**: Makes automated input less predictable
- **Realistic Testing**: Better simulates human behavior
- **Load Test Variation**: Creates more realistic load patterns
- **Anti-Bot Bypass**: Helps test applications with anti-automation measures

### Example: Human-like Typing

```ini
# Simulate natural typing rhythm
loop 10
    keydown H 60-100    # Variable key press duration
    pause 0.08-0.18     # Variable pause between keys
    keydown I 55-95
    pause 0.07-0.15
endloop
```

## Testing Your Application

### Load Testing Example

Create a `loadtest.ini` file:

```ini
# Rapid key press test
loop 1000
    keydown SPACE 50
    pause 0.01
endloop

# Sustained key hold test
loop 50
    keydown W 2000
    pause 0.5
endloop
```

Run with:

```bash
macro_tool.exe loadtest.ini
```

### UI Navigation Test

Create a `ui_test.ini` file:

```ini
# Navigate through menu items
keydown ALT 50
pause 0.1
keydown F 50  # File menu
pause 0.5
keydown DOWN 100
pause 0.2
keydown DOWN 100
pause 0.2
keydown ENTER 100
```

## Safety Features

- **ESC to Abort**: Press ESC at any time to immediately stop macro execution
- **Visual Feedback**: Console shows current action being performed
- **Delay Before Start**: 3-second countdown gives you time to switch to target app

## Troubleshooting

### Macro not working in certain applications

Some applications (especially games with anti-cheat) may block simulated input. Try:

- Running the tool as Administrator
- Using slightly longer key press durations
- Adding small pauses between key presses

### Keys not registering

- Ensure the target application has focus
- Increase key hold duration (some apps need longer press times)
- Check that the key name is spelled correctly in the config

### Compilation errors

- Ensure you have the Windows SDK installed
- Link with user32.lib (included in build commands)
- Use C++11 or later standard

## Security Note

This tool uses Windows SendInput API to simulate keyboard input. Some applications may detect this as automated input. Only use for testing your own applications or with permission.

## License

This tool is provided as-is for testing purposes. Use responsibly and only on applications you have permission to test.

## Support

For issues or questions:

1. Check that your config file syntax is correct
2. Verify the key names match the supported keys list
3. Try running as Administrator if keys aren't registering
4. Ensure no antivirus is blocking the tool

## Version History

- **v1.2** - Fixed decimal notation parsing (0.150 now correctly parsed as 150ms) and loop counter bug
- **v1.1** - Added random range support for pause and keydown commands
- **v1.0** - Initial release with basic macro support, loops, and extensive key mapping
