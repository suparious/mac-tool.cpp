#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>
#include <map>
#include <random>
#include <string_view>
#include <format>       // C++20/23: Should be available
#include <concepts>     // C++20: Available
#include <span>         // C++20: Available
#include <optional>     // C++17: Available
#include <filesystem>   // C++17: Available
#include <ranges>       // C++20: Available

namespace fs = std::filesystem;
namespace ranges = std::ranges;
using namespace std::chrono_literals;

// Strong typing with enum class (C++11 but better practice)
enum class CommandType {
    PAUSE,
    KEYDOWN,
    KEYPRESS,
    KEY,
    LOOP,
    ENDLOOP,
    UNKNOWN
};

// Modern struct with defaults (C++11/14/17 features)
struct Command {
    CommandType type{CommandType::UNKNOWN};
    std::string key{};
    std::chrono::milliseconds duration{0};
    std::chrono::milliseconds durationMin{0};
    std::chrono::milliseconds durationMax{0};
    bool useRandomRange{false};
};

// C++20: Using string_view for better performance
const std::map<std::string_view, BYTE> keyMap = {
    // Letters
    {"A", 0x41}, {"B", 0x42}, {"C", 0x43}, {"D", 0x44}, {"E", 0x45},
    {"F", 0x46}, {"G", 0x47}, {"H", 0x48}, {"I", 0x49}, {"J", 0x4A},
    {"K", 0x4B}, {"L", 0x4C}, {"M", 0x4D}, {"N", 0x4E}, {"O", 0x4F},
    {"P", 0x50}, {"Q", 0x51}, {"R", 0x52}, {"S", 0x53}, {"T", 0x54},
    {"U", 0x55}, {"V", 0x56}, {"W", 0x57}, {"X", 0x58}, {"Y", 0x59}, {"Z", 0x5A},
    
    // Numbers
    {"0", 0x30}, {"1", 0x31}, {"2", 0x32}, {"3", 0x33}, {"4", 0x34},
    {"5", 0x35}, {"6", 0x36}, {"7", 0x37}, {"8", 0x38}, {"9", 0x39},
    
    // Function keys
    {"F1", VK_F1}, {"F2", VK_F2}, {"F3", VK_F3}, {"F4", VK_F4},
    {"F5", VK_F5}, {"F6", VK_F6}, {"F7", VK_F7}, {"F8", VK_F8},
    {"F9", VK_F9}, {"F10", VK_F10}, {"F11", VK_F11}, {"F12", VK_F12},
    
    // Special keys
    {"SPACE", VK_SPACE}, {"ENTER", VK_RETURN}, {"RETURN", VK_RETURN},
    {"TAB", VK_TAB}, {"ESCAPE", VK_ESCAPE}, {"ESC", VK_ESCAPE},
    {"BACKSPACE", VK_BACK}, {"DELETE", VK_DELETE}, {"DEL", VK_DELETE},
    {"INSERT", VK_INSERT}, {"INS", VK_INSERT},
    {"HOME", VK_HOME}, {"END", VK_END},
    {"PAGEUP", VK_PRIOR}, {"PGUP", VK_PRIOR},
    {"PAGEDOWN", VK_NEXT}, {"PGDN", VK_NEXT},
    
    // Arrow keys
    {"UP", VK_UP}, {"DOWN", VK_DOWN}, {"LEFT", VK_LEFT}, {"RIGHT", VK_RIGHT},
    
    // Modifier keys
    {"SHIFT", VK_SHIFT}, {"LSHIFT", VK_LSHIFT}, {"RSHIFT", VK_RSHIFT},
    {"CTRL", VK_CONTROL}, {"CONTROL", VK_CONTROL},
    {"LCTRL", VK_LCONTROL}, {"RCTRL", VK_RCONTROL},
    {"ALT", VK_MENU}, {"LALT", VK_LMENU}, {"RALT", VK_RMENU},
    {"WIN", VK_LWIN}, {"WINDOWS", VK_LWIN}, {"LWIN", VK_LWIN}, {"RWIN", VK_RWIN},
    
    // Numpad
    {"NUMPAD0", VK_NUMPAD0}, {"NUMPAD1", VK_NUMPAD1}, {"NUMPAD2", VK_NUMPAD2},
    {"NUMPAD3", VK_NUMPAD3}, {"NUMPAD4", VK_NUMPAD4}, {"NUMPAD5", VK_NUMPAD5},
    {"NUMPAD6", VK_NUMPAD6}, {"NUMPAD7", VK_NUMPAD7}, {"NUMPAD8", VK_NUMPAD8},
    {"NUMPAD9", VK_NUMPAD9},
    {"MULTIPLY", VK_MULTIPLY}, {"ADD", VK_ADD}, {"SUBTRACT", VK_SUBTRACT},
    {"DECIMAL", VK_DECIMAL}, {"DIVIDE", VK_DIVIDE},
    
    // Punctuation and symbols
    {"COMMA", VK_OEM_COMMA}, {"PERIOD", VK_OEM_PERIOD}, {"DOT", VK_OEM_PERIOD},
    {"SEMICOLON", VK_OEM_1}, {"QUOTE", VK_OEM_7}, {"APOSTROPHE", VK_OEM_7},
    {"SLASH", VK_OEM_2}, {"BACKSLASH", VK_OEM_5},
    {"LEFTBRACKET", VK_OEM_4}, {"RIGHTBRACKET", VK_OEM_6},
    {"MINUS", VK_OEM_MINUS}, {"EQUALS", VK_OEM_PLUS}, {"EQUAL", VK_OEM_PLUS},
    {"GRAVE", VK_OEM_3}, {"TILDE", VK_OEM_3}
};

// C++17: [[nodiscard]] attribute
[[nodiscard]] constexpr std::string_view trim(std::string_view str) noexcept {
    constexpr std::string_view whitespace = " \t\r\n";
    const auto first = str.find_first_not_of(whitespace);
    if (first == std::string_view::npos) return {};
    const auto last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// C++20: Using ranges for transformation
[[nodiscard]] std::string toUpper(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    ranges::transform(str, std::back_inserter(result), 
                     [](unsigned char c) { return std::toupper(c); });
    return result;
}

// C++23: Check if string contains substring (fallback for older compilers)
[[nodiscard]] inline bool contains(std::string_view str, std::string_view substr) {
#if __cpp_lib_string_contains >= 202011L
    return std::string(str).contains(substr);
#else
    return str.find(substr) != std::string_view::npos;
#endif
}

// C++23: Check if string starts with prefix (fallback for older compilers)
[[nodiscard]] inline bool starts_with(std::string_view str, char c) {
#if __cpp_lib_starts_ends_with >= 201711L
    return str.starts_with(c);
#else
    return !str.empty() && str[0] == c;
#endif
}

// C++17: Using optional for better null handling
struct ParsedValue {
    int value;
    int minVal;
    int maxVal;
    bool isRange;
};

[[nodiscard]] std::optional<ParsedValue> parseValueOrRange(std::string_view str) {
    if (const auto dashPos = str.find('-'); dashPos != std::string_view::npos && dashPos > 0) {
        // Parse as range
        const auto minStr = str.substr(0, dashPos);
        const auto maxStr = str.substr(dashPos + 1);
        
        try {
            float minFloat = std::stof(std::string(minStr));
            float maxFloat = std::stof(std::string(maxStr));
            
            int minVal = static_cast<int>(minFloat);
            int maxVal = static_cast<int>(maxFloat);
            
            if (minVal > maxVal) {
                std::swap(minVal, maxVal);
            }
            
            return ParsedValue{minVal, minVal, maxVal, true};
        }
        catch (...) {
            return std::nullopt;
        }
    }
    else {
        // Parse as single value
        try {
            float floatValue = std::stof(std::string(str));
            int value = static_cast<int>(floatValue);
            return ParsedValue{value, value, value, false};
        }
        catch (...) {
            return std::nullopt;
        }
    }
}

// Modern random number generator
class RandomGenerator {
private:
    std::random_device rd;
    std::mt19937 gen{rd()};
    
public:
    [[nodiscard]] int getInRange(int min, int max) {
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
    
    [[nodiscard]] auto getInRange(std::chrono::milliseconds min, std::chrono::milliseconds max) {
        return std::chrono::milliseconds{getInRange(static_cast<int>(min.count()), 
                                                    static_cast<int>(max.count()))};
    }
};

// Parse pause command with optional
[[nodiscard]] std::optional<Command> parsePauseCommand(std::string_view durationStr, int lineNumber) {
    Command cmd{.type = CommandType::PAUSE};
    
    if (const auto dashPos = durationStr.find('-'); dashPos != std::string_view::npos && dashPos > 0) {
        // Range
        try {
            float minSeconds = std::stof(std::string(durationStr.substr(0, dashPos)));
            float maxSeconds = std::stof(std::string(durationStr.substr(dashPos + 1)));
            
            cmd.durationMin = std::chrono::milliseconds{static_cast<int>(minSeconds * 1000)};
            cmd.durationMax = std::chrono::milliseconds{static_cast<int>(maxSeconds * 1000)};
            
            if (cmd.durationMin > cmd.durationMax) {
                std::swap(cmd.durationMin, cmd.durationMax);
            }
            
            cmd.duration = cmd.durationMin;
            cmd.useRandomRange = true;
        }
        catch (const std::exception& e) {
            std::cerr << std::format("Invalid pause duration at line {}: {}\n", lineNumber, e.what());
            return std::nullopt;
        }
    }
    else {
        // Single value
        try {
            float seconds = std::stof(std::string(durationStr));
            cmd.duration = std::chrono::milliseconds{static_cast<int>(seconds * 1000)};
            cmd.durationMin = cmd.duration;
            cmd.durationMax = cmd.duration;
            cmd.useRandomRange = false;
        }
        catch (const std::exception& e) {
            std::cerr << std::format("Invalid pause duration at line {}: {}\n", lineNumber, e.what());
            return std::nullopt;
        }
    }
    
    return cmd;
}

// C++17/20: Using filesystem and optional
[[nodiscard]] std::optional<std::vector<Command>> parseConfig(const fs::path& filename) {
    if (!fs::exists(filename)) {
        std::cerr << std::format("Config file does not exist: {}\n", filename.string());
        return std::nullopt;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << std::format("Could not open config file: {}\n", filename.string());
        return std::nullopt;
    }
    
    std::vector<Command> commands;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        const auto trimmedLine = trim(line);
        
        // Skip empty lines and comments
        if (trimmedLine.empty() || starts_with(trimmedLine, '#') || starts_with(trimmedLine, ';')) {
            continue;
        }
        
        std::istringstream iss(std::string(trimmedLine));
        std::string commandType;
        iss >> commandType;
        
        commandType = toUpper(commandType);
        
        if (commandType == "PAUSE") {
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << std::format("Warning: Invalid pause duration at line {}: {}\n", lineNumber, line);
                continue;
            }
            
            if (auto cmd = parsePauseCommand(durationStr, lineNumber)) {
                commands.push_back(*cmd);
            }
        }
        else if (commandType == "KEYDOWN" || commandType == "KEYPRESS") {
            Command cmd{.type = commandType == "KEYDOWN" ? CommandType::KEYDOWN : CommandType::KEYPRESS};
            
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << std::format("Warning: Missing key at line {}: {}\n", lineNumber, line);
                continue;
            }
            
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << std::format("Warning: Invalid key duration at line {}: {}\n", lineNumber, line);
                continue;
            }
            
            if (auto parsed = parseValueOrRange(durationStr)) {
                cmd.duration = std::chrono::milliseconds{parsed->value};
                cmd.durationMin = std::chrono::milliseconds{parsed->minVal};
                cmd.durationMax = std::chrono::milliseconds{parsed->maxVal};
                cmd.useRandomRange = parsed->isRange;
                cmd.key = toUpper(cmd.key);
                commands.push_back(cmd);
            } else {
                std::cerr << std::format("Warning: Invalid key duration at line {}: {}\n", lineNumber, line);
            }
        }
        else if (commandType == "KEY") {
            Command cmd{
                .type = CommandType::KEY,
                .key = "",
                .duration = 50ms,
                .durationMin = 50ms,
                .durationMax = 50ms,
                .useRandomRange = false
            };
            
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << std::format("Warning: Missing key at line {}: {}\n", lineNumber, line);
                continue;
            }
            cmd.key = toUpper(cmd.key);
            commands.push_back(cmd);
        }
        else if (commandType == "LOOP") {
            int loopCount;
            if (!(iss >> loopCount)) {
                std::cerr << std::format("Warning: Invalid loop count at line {}: {}\n", lineNumber, line);
                continue;
            }
            
            Command cmd{
                .type = CommandType::LOOP,
                .duration = std::chrono::milliseconds{loopCount}
            };
            commands.push_back(cmd);
        }
        else if (commandType == "ENDLOOP") {
            commands.push_back(Command{.type = CommandType::ENDLOOP});
        }
        else {
            std::cerr << std::format("Warning: Unknown command at line {}: {}\n", lineNumber, line);
        }
    }
    
    return commands;
}

// Simulate key press with chrono durations
void pressKey(BYTE vkCode, std::chrono::milliseconds duration) {
    INPUT input[2] = {};
    
    // Key down
    input[0].type = INPUT_KEYBOARD;
    input[0].ki.wVk = vkCode;
    input[0].ki.dwFlags = 0;
    
    // Key up
    input[1].type = INPUT_KEYBOARD;
    input[1].ki.wVk = vkCode;
    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
    
    // Send key down
    SendInput(1, &input[0], sizeof(INPUT));
    
    // Hold for specified duration
    std::this_thread::sleep_for(duration);
    
    // Send key up
    SendInput(1, &input[1], sizeof(INPUT));
}

// C++20: Using span for array access
void executeMacro(std::span<const Command> commands) {
    std::vector<int> loopStack;
    RandomGenerator rng;
    
    for (size_t i = 0; i < commands.size(); i++) {
        const auto& cmd = commands[i];
        
        // Check for abort key (ESC)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            std::cout << "Macro aborted by user (ESC pressed)\n";
            break;
        }
        
        switch (cmd.type) {
            case CommandType::PAUSE: {
                auto actualDuration = cmd.duration;
                if (cmd.useRandomRange) {
                    actualDuration = rng.getInRange(cmd.durationMin, cmd.durationMax);
                    std::cout << std::format("Pausing for {:.3f} seconds (random from {:.3f}-{:.3f})...\n",
                                            actualDuration.count() / 1000.0,
                                            cmd.durationMin.count() / 1000.0,
                                            cmd.durationMax.count() / 1000.0);
                } else {
                    std::cout << std::format("Pausing for {:.3f} seconds...\n", 
                                            actualDuration.count() / 1000.0);
                }
                std::this_thread::sleep_for(actualDuration);
                break;
            }
            
            case CommandType::KEYDOWN:
            case CommandType::KEYPRESS:
            case CommandType::KEY: {
                const auto upperKey = toUpper(cmd.key);
                if (auto it = keyMap.find(upperKey); it != keyMap.end()) {
                    auto actualDuration = cmd.duration;
                    if (cmd.useRandomRange) {
                        actualDuration = rng.getInRange(cmd.durationMin, cmd.durationMax);
                        std::cout << std::format("Pressing key: {} for {}ms (random from {}-{}ms)\n",
                                               cmd.key, actualDuration.count(),
                                               cmd.durationMin.count(), cmd.durationMax.count());
                    } else {
                        std::cout << std::format("Pressing key: {} for {}ms\n", 
                                               cmd.key, actualDuration.count());
                    }
                    pressKey(it->second, actualDuration);
                }
                else {
                    std::cerr << std::format("Unknown key: {}\n", cmd.key);
                }
                break;
            }
            
            case CommandType::LOOP: {
                loopStack.push_back(static_cast<int>(cmd.duration.count()));
                std::cout << std::format("Starting loop ({} iterations)\n", cmd.duration.count());
                break;
            }
            
            case CommandType::ENDLOOP: {
                if (!loopStack.empty()) {
                    auto& count = loopStack.back();
                    count--;
                    if (count > 0) {
                        // Find matching LOOP command
                        int loopLevel = 1;
                        for (auto j = static_cast<int>(i) - 1; j >= 0; j--) {
                            if (commands[j].type == CommandType::ENDLOOP) {
                                loopLevel++;
                            }
                            else if (commands[j].type == CommandType::LOOP) {
                                loopLevel--;
                                if (loopLevel == 0) {
                                    i = j;
                                    break;
                                }
                            }
                        }
                    }
                    else {
                        loopStack.pop_back();
                        std::cout << "Loop completed\n";
                    }
                }
                break;
            }
            
            default:
                break;
        }
    }
}

int main(int argc, char* argv[]) {
    // C++20: Using span for argv
    std::span<char*> args(argv, argc);
    
    fs::path configFile = "macro.ini";
    if (args.size() > 1) {
        configFile = args[1];
    }
    
    std::cout << "================================\n";
    std::cout << "Keyboard Macro Tool v2.0 (C++20/23)\n";
    std::cout << "================================\n";
    std::cout << std::format("Loading config from: {}\n", configFile.string());
    std::cout << "Press ESC at any time to abort the macro\n\n";
    
    // Parse config file with optional
    auto commandsOpt = parseConfig(configFile);
    if (!commandsOpt) {
        return 1;
    }
    
    auto& commands = *commandsOpt;
    
    if (commands.empty()) {
        std::cerr << "No valid commands found in config file\n";
        return 1;
    }
    
    std::cout << std::format("Loaded {} commands\n", commands.size());
    std::cout << "Starting in 3 seconds...\n\n";
    
    // Give user time to switch to target application
    std::this_thread::sleep_for(3s);
    
    // Execute macro with span
    executeMacro(commands);
    
    std::cout << "\nMacro execution completed!\n";
    
    return 0;
}
