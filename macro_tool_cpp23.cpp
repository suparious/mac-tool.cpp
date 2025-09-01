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
#include <ctime>
#include <ranges>       // C++23: ranges support
#include <expected>     // C++23: std::expected for better error handling
#include <print>        // C++23: std::print for formatted output
#include <string_view>  // Better string handling
#include <format>       // C++20/23: Better formatting
#include <concepts>     // C++20/23: Concepts for type constraints
#include <span>         // C++20: Safer array views
#include <optional>     // Better null handling
#include <filesystem>   // Better file handling

namespace fs = std::filesystem;
namespace ranges = std::ranges;
namespace views = std::views;
using namespace std::chrono_literals; // C++14 but useful with C++23

// C++23: Using std::expected for better error handling
template<typename T>
using Result = std::expected<T, std::string>;

// C++23: constexpr std::map is now possible
constexpr auto initKeyMap() {
    std::map<std::string_view, BYTE> map;
    // Letters
    map["A"] = 0x41; map["B"] = 0x42; map["C"] = 0x43; map["D"] = 0x44; map["E"] = 0x45;
    map["F"] = 0x46; map["G"] = 0x47; map["H"] = 0x48; map["I"] = 0x49; map["J"] = 0x4A;
    map["K"] = 0x4B; map["L"] = 0x4C; map["M"] = 0x4D; map["N"] = 0x4E; map["O"] = 0x4F;
    map["P"] = 0x50; map["Q"] = 0x51; map["R"] = 0x52; map["S"] = 0x53; map["T"] = 0x54;
    map["U"] = 0x55; map["V"] = 0x56; map["W"] = 0x57; map["X"] = 0x58; map["Y"] = 0x59;
    map["Z"] = 0x5A;
    
    // Numbers
    map["0"] = 0x30; map["1"] = 0x31; map["2"] = 0x32; map["3"] = 0x33; map["4"] = 0x34;
    map["5"] = 0x35; map["6"] = 0x36; map["7"] = 0x37; map["8"] = 0x38; map["9"] = 0x39;
    
    // Function keys
    map["F1"] = VK_F1; map["F2"] = VK_F2; map["F3"] = VK_F3; map["F4"] = VK_F4;
    map["F5"] = VK_F5; map["F6"] = VK_F6; map["F7"] = VK_F7; map["F8"] = VK_F8;
    map["F9"] = VK_F9; map["F10"] = VK_F10; map["F11"] = VK_F11; map["F12"] = VK_F12;
    
    // Special keys
    map["SPACE"] = VK_SPACE; map["ENTER"] = VK_RETURN; map["RETURN"] = VK_RETURN;
    map["TAB"] = VK_TAB; map["ESCAPE"] = VK_ESCAPE; map["ESC"] = VK_ESCAPE;
    map["BACKSPACE"] = VK_BACK; map["DELETE"] = VK_DELETE; map["DEL"] = VK_DELETE;
    map["INSERT"] = VK_INSERT; map["INS"] = VK_INSERT;
    map["HOME"] = VK_HOME; map["END"] = VK_END;
    map["PAGEUP"] = VK_PRIOR; map["PGUP"] = VK_PRIOR;
    map["PAGEDOWN"] = VK_NEXT; map["PGDN"] = VK_NEXT;
    
    // Arrow keys
    map["UP"] = VK_UP; map["DOWN"] = VK_DOWN; map["LEFT"] = VK_LEFT; map["RIGHT"] = VK_RIGHT;
    
    // Modifier keys
    map["SHIFT"] = VK_SHIFT; map["LSHIFT"] = VK_LSHIFT; map["RSHIFT"] = VK_RSHIFT;
    map["CTRL"] = VK_CONTROL; map["CONTROL"] = VK_CONTROL;
    map["LCTRL"] = VK_LCONTROL; map["RCTRL"] = VK_RCONTROL;
    map["ALT"] = VK_MENU; map["LALT"] = VK_LMENU; map["RALT"] = VK_RMENU;
    map["WIN"] = VK_LWIN; map["WINDOWS"] = VK_LWIN; map["LWIN"] = VK_LWIN; map["RWIN"] = VK_RWIN;
    
    // Numpad
    map["NUMPAD0"] = VK_NUMPAD0; map["NUMPAD1"] = VK_NUMPAD1; map["NUMPAD2"] = VK_NUMPAD2;
    map["NUMPAD3"] = VK_NUMPAD3; map["NUMPAD4"] = VK_NUMPAD4; map["NUMPAD5"] = VK_NUMPAD5;
    map["NUMPAD6"] = VK_NUMPAD6; map["NUMPAD7"] = VK_NUMPAD7; map["NUMPAD8"] = VK_NUMPAD8;
    map["NUMPAD9"] = VK_NUMPAD9;
    map["MULTIPLY"] = VK_MULTIPLY; map["ADD"] = VK_ADD; map["SUBTRACT"] = VK_SUBTRACT;
    map["DECIMAL"] = VK_DECIMAL; map["DIVIDE"] = VK_DIVIDE;
    
    // Punctuation and symbols
    map["COMMA"] = VK_OEM_COMMA; map["PERIOD"] = VK_OEM_PERIOD; map["DOT"] = VK_OEM_PERIOD;
    map["SEMICOLON"] = VK_OEM_1; map["QUOTE"] = VK_OEM_7; map["APOSTROPHE"] = VK_OEM_7;
    map["SLASH"] = VK_OEM_2; map["BACKSLASH"] = VK_OEM_5;
    map["LEFTBRACKET"] = VK_OEM_4; map["RIGHTBRACKET"] = VK_OEM_6;
    map["MINUS"] = VK_OEM_MINUS; map["EQUALS"] = VK_OEM_PLUS; map["EQUAL"] = VK_OEM_PLUS;
    map["GRAVE"] = VK_OEM_3; map["TILDE"] = VK_OEM_3;
    
    return map;
}

inline constexpr auto keyMap = initKeyMap();

// C++23: Structured binding with better enum support
enum class CommandType {
    PAUSE,
    KEYDOWN,
    KEYPRESS,
    KEY,
    LOOP,
    ENDLOOP,
    UNKNOWN
};

// C++23: Using C++20 designated initializers with C++23 improvements
struct Command {
    CommandType type{CommandType::UNKNOWN};
    std::string key{};
    std::chrono::milliseconds duration{0};
    std::chrono::milliseconds durationMin{0};
    std::chrono::milliseconds durationMax{0};
    bool useRandomRange{false};
    
    // C++23: Deducing this for method chaining
    auto& setDuration(this auto& self, std::chrono::milliseconds d) {
        self.duration = d;
        return self;
    }
    
    auto& setRange(this auto& self, std::chrono::milliseconds min, std::chrono::milliseconds max) {
        self.durationMin = min;
        self.durationMax = max;
        self.useRandomRange = true;
        return self;
    }
};

// C++23: std::string::contains() is now available
[[nodiscard]] constexpr std::string_view trim(std::string_view str) noexcept {
    constexpr std::string_view whitespace = " \t\r\n";
    const auto first = str.find_first_not_of(whitespace);
    if (first == std::string_view::npos) return {};
    const auto last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

// C++23: Using ranges for transformation
[[nodiscard]] std::string toUpper(std::string_view str) {
    std::string result;
    result.reserve(str.size());
    ranges::transform(str, std::back_inserter(result), 
                     [](unsigned char c) { return std::toupper(c); });
    return result;
}

// C++23: Using std::expected for error handling
struct ParsedValue {
    int value;
    int minVal;
    int maxVal;
    bool isRange;
};

[[nodiscard]] Result<ParsedValue> parseValueOrRange(std::string_view str) {
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
            
            return ParsedValue{.value = minVal, .minVal = minVal, .maxVal = maxVal, .isRange = true};
        }
        catch (const std::exception& e) {
            return std::unexpected(std::format("Failed to parse range: {}", e.what()));
        }
    }
    else {
        // Parse as single value
        try {
            float floatValue = std::stof(std::string(str));
            int value = static_cast<int>(floatValue);
            return ParsedValue{.value = value, .minVal = value, .maxVal = value, .isRange = false};
        }
        catch (const std::exception& e) {
            return std::unexpected(std::format("Failed to parse value: {}", e.what()));
        }
    }
}

// C++23: Using std::random_device properly with C++23 improvements
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

// C++23: Using concepts for type constraints
template<typename T>
concept Numeric = std::integral<T> || std::floating_point<T>;

// Parse duration with better error handling
[[nodiscard]] Result<Command> parsePauseCommand(std::string_view durationStr, int lineNumber) {
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
            return std::unexpected(std::format("Invalid pause duration at line {}: {}", lineNumber, e.what()));
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
            return std::unexpected(std::format("Invalid pause duration at line {}: {}", lineNumber, e.what()));
        }
    }
    
    return cmd;
}

// C++23: Using std::expected for parseConfig
[[nodiscard]] Result<std::vector<Command>> parseConfig(const fs::path& filename) {
    if (!fs::exists(filename)) {
        return std::unexpected(std::format("Config file does not exist: {}", filename.string()));
    }
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        return std::unexpected(std::format("Could not open config file: {}", filename.string()));
    }
    
    std::vector<Command> commands;
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        const auto trimmedLine = trim(line);
        
        // Skip empty lines and comments
        if (trimmedLine.empty() || trimmedLine.starts_with('#') || trimmedLine.starts_with(';')) {
            continue;
        }
        
        std::istringstream iss(std::string(trimmedLine));
        std::string commandType;
        iss >> commandType;
        
        commandType = toUpper(commandType);
        
        // C++23: Using if-constexpr with pattern matching style
        if (commandType == "PAUSE") {
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::println(std::cerr, "Warning: Invalid pause duration at line {}: {}", lineNumber, line);
                continue;
            }
            
            if (auto result = parsePauseCommand(durationStr, lineNumber); result) {
                commands.push_back(result.value());
            } else {
                std::println(std::cerr, "Warning: {}", result.error());
            }
        }
        else if (commandType == "KEYDOWN" || commandType == "KEYPRESS") {
            Command cmd{.type = commandType == "KEYDOWN" ? CommandType::KEYDOWN : CommandType::KEYPRESS};
            
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::println(std::cerr, "Warning: Missing key at line {}: {}", lineNumber, line);
                continue;
            }
            
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::println(std::cerr, "Warning: Invalid key duration at line {}: {}", lineNumber, line);
                continue;
            }
            
            if (auto parsed = parseValueOrRange(durationStr); parsed) {
                const auto& val = parsed.value();
                cmd.duration = std::chrono::milliseconds{val.value};
                cmd.durationMin = std::chrono::milliseconds{val.minVal};
                cmd.durationMax = std::chrono::milliseconds{val.maxVal};
                cmd.useRandomRange = val.isRange;
                cmd.key = toUpper(cmd.key);
                commands.push_back(cmd);
            } else {
                std::println(std::cerr, "Warning: Invalid key duration at line {}: {}", lineNumber, parsed.error());
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
                std::println(std::cerr, "Warning: Missing key at line {}: {}", lineNumber, line);
                continue;
            }
            cmd.key = toUpper(cmd.key);
            commands.push_back(cmd);
        }
        else if (commandType == "LOOP") {
            int loopCount;
            if (!(iss >> loopCount)) {
                std::println(std::cerr, "Warning: Invalid loop count at line {}: {}", lineNumber, line);
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
            std::println(std::cerr, "Warning: Unknown command at line {}: {}", lineNumber, line);
        }
    }
    
    return commands;
}

// Simulate key press with better timing
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

// C++23: Using jthread for better thread management
void executeMacro(std::span<const Command> commands, std::stop_token stopToken) {
    std::vector<int> loopStack;
    RandomGenerator rng;
    
    for (size_t i = 0; i < commands.size() && !stopToken.stop_requested(); i++) {
        const auto& cmd = commands[i];
        
        // Check for abort key (ESC)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            std::println("Macro aborted by user (ESC pressed)");
            break;
        }
        
        switch (cmd.type) {
            case CommandType::PAUSE: {
                auto actualDuration = cmd.duration;
                if (cmd.useRandomRange) {
                    actualDuration = rng.getInRange(cmd.durationMin, cmd.durationMax);
                    std::println("Pausing for {:.3f} seconds (random from {:.3f}-{:.3f})...", 
                               actualDuration.count() / 1000.0,
                               cmd.durationMin.count() / 1000.0,
                               cmd.durationMax.count() / 1000.0);
                } else {
                    std::println("Pausing for {:.3f} seconds...", actualDuration.count() / 1000.0);
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
                        std::println("Pressing key: {} for {}ms (random from {}-{}ms)",
                                   cmd.key, actualDuration.count(),
                                   cmd.durationMin.count(), cmd.durationMax.count());
                    } else {
                        std::println("Pressing key: {} for {}ms", cmd.key, actualDuration.count());
                    }
                    pressKey(it->second, actualDuration);
                }
                else {
                    std::println(std::cerr, "Unknown key: {}", cmd.key);
                }
                break;
            }
            
            case CommandType::LOOP: {
                loopStack.push_back(static_cast<int>(cmd.duration.count()));
                std::println("Starting loop ({} iterations)", cmd.duration.count());
                break;
            }
            
            case CommandType::ENDLOOP: {
                if (!loopStack.empty()) {
                    auto& count = loopStack.back();
                    count--;
                    if (count > 0) {
                        // Find matching LOOP command using ranges
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
                        std::println("Loop completed");
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
    // C++23: Using std::span for argv
    std::span<char*> args(argv, argc);
    
    fs::path configFile = "macro.ini";
    if (args.size() > 1) {
        configFile = args[1];
    }
    
    std::println("================================");
    std::println("Keyboard Macro Tool v2.0 (C++23)");
    std::println("================================");
    std::println("Loading config from: {}", configFile.string());
    std::println("Press ESC at any time to abort the macro");
    std::println("");
    
    // Parse config file with better error handling
    auto commandsResult = parseConfig(configFile);
    if (!commandsResult) {
        std::println(std::cerr, "Error: {}", commandsResult.error());
        return 1;
    }
    
    auto& commands = commandsResult.value();
    
    if (commands.empty()) {
        std::println(std::cerr, "No valid commands found in config file");
        return 1;
    }
    
    std::println("Loaded {} commands", commands.size());
    std::println("Starting in 3 seconds...");
    std::println("");
    
    // Give user time to switch to target application
    std::this_thread::sleep_for(3s);
    
    // C++23: Using jthread for automatic cleanup
    std::jthread macroThread([&commands](std::stop_token st) {
        executeMacro(commands, st);
    });
    
    // Wait for completion
    macroThread.join();
    
    std::println("");
    std::println("Macro execution completed!");
    
    return 0;
}
