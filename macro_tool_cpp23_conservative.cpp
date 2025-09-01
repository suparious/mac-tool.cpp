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
#include <optional>
#include <filesystem>
#include <span>
#include <ranges>
#include <iomanip>

namespace fs = std::filesystem;
namespace ranges = std::ranges;
using namespace std::chrono_literals;

// Command type enum for better type safety
enum class CommandType {
    PAUSE,
    KEYDOWN,
    KEYPRESS,
    KEY,
    LOOP,
    ENDLOOP,
    UNKNOWN
};

// Improved Command structure
struct Command {
    CommandType type = CommandType::UNKNOWN;
    std::string key;
    std::chrono::milliseconds duration{0};
    std::chrono::milliseconds durationMin{0};
    std::chrono::milliseconds durationMax{0};
    bool useRandomRange = false;
};

// Virtual key code mapping - using string_view for efficiency
const std::map<std::string, BYTE> keyMap = {
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
    {"CTRL", VK_CONTROL}, {"CONTROL", VK_CONTROL}, {"LCTRL", VK_LCONTROL}, {"RCTRL", VK_RCONTROL},
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

// C++23: Using string_view for efficiency
[[nodiscard]] std::string_view trim(std::string_view str) noexcept {
    const auto first = str.find_first_not_of(" \t\r\n");
    if (first == std::string_view::npos) return {};
    const auto last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// C++20/23: Using ranges
[[nodiscard]] std::string toUpper(std::string_view str) {
    std::string result(str);
    ranges::transform(result, result.begin(), 
                     [](unsigned char c) { return std::toupper(c); });
    return result;
}

// Result type for better error handling
struct ParsedValue {
    int value;
    int minVal;
    int maxVal;
    bool isRange;
};

// C++17/20: Using optional for error handling
[[nodiscard]] std::optional<ParsedValue> parseValueOrRange(std::string_view str) {
    const auto dashPos = str.find('-');
    
    if (dashPos != std::string_view::npos && dashPos > 0) {
        // Parse as range
        try {
            float minFloat = std::stof(std::string(str.substr(0, dashPos)));
            float maxFloat = std::stof(std::string(str.substr(dashPos + 1)));
            
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

// Random generator class
class RandomGenerator {
private:
    std::random_device rd;
    std::mt19937 gen;
    
public:
    RandomGenerator() : gen(rd()) {}
    
    [[nodiscard]] int getInRange(int min, int max) {
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
    
    [[nodiscard]] std::chrono::milliseconds getInRange(std::chrono::milliseconds min, 
                                                        std::chrono::milliseconds max) {
        return std::chrono::milliseconds{
            getInRange(static_cast<int>(min.count()), static_cast<int>(max.count()))
        };
    }
};

// Helper function for formatted output (since std::format might not be available)
template<typename T>
void printFormatted(const std::string& format, const T& value) {
    std::cout << std::fixed << std::setprecision(3);
    size_t pos = format.find("{}");
    if (pos != std::string::npos) {
        std::cout << format.substr(0, pos) << value << format.substr(pos + 2);
    } else {
        std::cout << format;
    }
}

// Parse pause command
[[nodiscard]] std::optional<Command> parsePauseCommand(std::string_view durationStr, int lineNumber) {
    Command cmd;
    cmd.type = CommandType::PAUSE;
    
    const auto dashPos = durationStr.find('-');
    if (dashPos != std::string_view::npos && dashPos > 0) {
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
            std::cerr << "Warning: Invalid pause duration at line " << lineNumber 
                     << ": " << e.what() << "\n";
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
            std::cerr << "Warning: Invalid pause duration at line " << lineNumber 
                     << ": " << e.what() << "\n";
            return std::nullopt;
        }
    }
    
    return cmd;
}

// Parse config with filesystem
[[nodiscard]] std::optional<std::vector<Command>> parseConfig(const fs::path& filename) {
    if (!fs::exists(filename)) {
        std::cerr << "Error: Config file does not exist: " << filename.string() << "\n";
        return std::nullopt;
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file: " << filename.string() << "\n";
        return std::nullopt;
    }
    
    std::vector<Command> commands;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        const auto trimmedLine = trim(line);
        
        // Skip empty lines and comments
        if (trimmedLine.empty() || trimmedLine[0] == '#' || trimmedLine[0] == ';') {
            continue;
        }
        
        std::istringstream iss(std::string(trimmedLine));
        std::string commandTypeStr;
        iss >> commandTypeStr;
        
        commandTypeStr = toUpper(commandTypeStr);
        
        if (commandTypeStr == "PAUSE") {
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << "Warning: Invalid pause duration at line " << lineNumber 
                         << ": " << line << "\n";
                continue;
            }
            
            if (auto cmd = parsePauseCommand(durationStr, lineNumber)) {
                commands.push_back(*cmd);
            }
        }
        else if (commandTypeStr == "KEYDOWN" || commandTypeStr == "KEYPRESS") {
            Command cmd;
            cmd.type = commandTypeStr == "KEYDOWN" ? CommandType::KEYDOWN : CommandType::KEYPRESS;
            
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << "Warning: Missing key at line " << lineNumber << ": " << line << "\n";
                continue;
            }
            
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << "Warning: Invalid key duration at line " << lineNumber 
                         << ": " << line << "\n";
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
                std::cerr << "Warning: Invalid key duration at line " << lineNumber 
                         << ": " << line << "\n";
            }
        }
        else if (commandTypeStr == "KEY") {
            Command cmd;
            cmd.type = CommandType::KEY;
            cmd.duration = 50ms;
            cmd.durationMin = 50ms;
            cmd.durationMax = 50ms;
            cmd.useRandomRange = false;
            
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << "Warning: Missing key at line " << lineNumber << ": " << line << "\n";
                continue;
            }
            cmd.key = toUpper(cmd.key);
            commands.push_back(cmd);
        }
        else if (commandTypeStr == "LOOP") {
            int loopCount;
            if (!(iss >> loopCount)) {
                std::cerr << "Warning: Invalid loop count at line " << lineNumber 
                         << ": " << line << "\n";
                continue;
            }
            
            Command cmd;
            cmd.type = CommandType::LOOP;
            cmd.duration = std::chrono::milliseconds{loopCount};
            commands.push_back(cmd);
        }
        else if (commandTypeStr == "ENDLOOP") {
            Command cmd;
            cmd.type = CommandType::ENDLOOP;
            commands.push_back(cmd);
        }
        else {
            std::cerr << "Warning: Unknown command at line " << lineNumber << ": " << line << "\n";
        }
    }
    
    return commands;
}

// Simulate key press with chrono duration
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
    
    SendInput(1, &input[0], sizeof(INPUT));
    std::this_thread::sleep_for(duration);
    SendInput(1, &input[1], sizeof(INPUT));
}

// Execute macro with span (C++20 feature)
void executeMacro(const std::vector<Command>& commands) {
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
                    std::cout << "Pausing for " << std::fixed << std::setprecision(3) 
                             << actualDuration.count() / 1000.0 << " seconds (random from "
                             << cmd.durationMin.count() / 1000.0 << "-"
                             << cmd.durationMax.count() / 1000.0 << ")...\n";
                } else {
                    std::cout << "Pausing for " << std::fixed << std::setprecision(3)
                             << actualDuration.count() / 1000.0 << " seconds...\n";
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
                        std::cout << "Pressing key: " << cmd.key << " for " 
                                 << actualDuration.count() << "ms (random from "
                                 << cmd.durationMin.count() << "-" 
                                 << cmd.durationMax.count() << "ms)\n";
                    } else {
                        std::cout << "Pressing key: " << cmd.key << " for " 
                                 << actualDuration.count() << "ms\n";
                    }
                    pressKey(it->second, actualDuration);
                } else {
                    std::cerr << "Unknown key: " << cmd.key << "\n";
                }
                break;
            }
            
            case CommandType::LOOP: {
                loopStack.push_back(static_cast<int>(cmd.duration.count()));
                std::cout << "Starting loop (" << cmd.duration.count() << " iterations)\n";
                break;
            }
            
            case CommandType::ENDLOOP: {
                if (!loopStack.empty()) {
                    auto& count = loopStack.back();
                    count--;
                    if (count > 0) {
                        // Find matching LOOP command
                        int loopLevel = 1;
                        for (int j = static_cast<int>(i) - 1; j >= 0; j--) {
                            if (commands[j].type == CommandType::ENDLOOP) {
                                loopLevel++;
                            } else if (commands[j].type == CommandType::LOOP) {
                                loopLevel--;
                                if (loopLevel == 0) {
                                    i = j;
                                    break;
                                }
                            }
                        }
                    } else {
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
    fs::path configFile = "macro.ini";
    if (argc > 1) {
        configFile = argv[1];
    }
    
    std::cout << "================================\n";
    std::cout << "Keyboard Macro Tool v2.0 (C++23)\n";
    std::cout << "================================\n";
    std::cout << "Loading config from: " << configFile.string() << "\n";
    std::cout << "Press ESC at any time to abort the macro\n\n";
    
    // Parse config file with better error handling
    auto commandsOpt = parseConfig(configFile);
    if (!commandsOpt) {
        return 1;
    }
    
    auto& commands = *commandsOpt;
    
    if (commands.empty()) {
        std::cerr << "No valid commands found in config file\n";
        return 1;
    }
    
    std::cout << "Loaded " << commands.size() << " commands\n";
    std::cout << "Starting in 3 seconds...\n\n";
    
    // Give user time to switch to target application
    std::this_thread::sleep_for(3s);
    
    // Execute macro
    executeMacro(commands);
    
    std::cout << "\nMacro execution completed!\n";
    
    return 0;
}
