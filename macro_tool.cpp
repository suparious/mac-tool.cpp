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

// Virtual key code mapping for common keys
std::map<std::string, BYTE> keyMap = {
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

struct Command {
    std::string type;
    std::string key;
    int duration;
    int durationMin;  // For random range
    int durationMax;  // For random range
    bool useRandomRange;
};

// Trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Convert string to uppercase
std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Parse a value that might be a range (e.g., "100" or "50-150" or "0.5" for decimals)
bool parseValueOrRange(const std::string& str, int& value, int& minVal, int& maxVal, bool& isRange) {
    size_t dashPos = str.find('-');
    
    // Check if this is a range (has a dash that's not at the beginning)
    if (dashPos != std::string::npos && dashPos > 0) {
        // Parse as range
        std::string minStr = str.substr(0, dashPos);
        std::string maxStr = str.substr(dashPos + 1);
        
        try {
            // Parse as float first to handle decimals, then convert to int
            float minFloat = std::stof(minStr);
            float maxFloat = std::stof(maxStr);
            
            minVal = static_cast<int>(minFloat);
            maxVal = static_cast<int>(maxFloat);
            
            if (minVal > maxVal) {
                // Swap if min > max
                std::swap(minVal, maxVal);
            }
            
            isRange = true;
            value = minVal; // Default value
            return true;
        }
        catch (...) {
            return false;
        }
    }
    else {
        // Parse as single value
        try {
            // Parse as float first to handle decimals like 0.150
            float floatValue = std::stof(str);
            value = static_cast<int>(floatValue);
            minVal = value;
            maxVal = value;
            isRange = false;
            return true;
        }
        catch (...) {
            return false;
        }
    }
}

// Random number generator
std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));

int getRandomInRange(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

// Parse the config file
std::vector<Command> parseConfig(const std::string& filename) {
    std::vector<Command> commands;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filename);
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';') {
            continue;
        }
        
        std::istringstream iss(line);
        std::string commandType;
        iss >> commandType;
        
        commandType = toUpper(commandType);
        
        Command cmd;
        cmd.type = commandType;
        
        if (commandType == "PAUSE") {
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << "Warning: Invalid pause duration at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            
            // Parse pause duration - handle both decimals and ranges
            // For pause, we expect seconds (possibly with decimals)
            size_t dashPos = durationStr.find('-');
            if (dashPos != std::string::npos && dashPos > 0) {
                // This is a range
                std::string minStr = durationStr.substr(0, dashPos);
                std::string maxStr = durationStr.substr(dashPos + 1);
                try {
                    float minSeconds = std::stof(minStr);
                    float maxSeconds = std::stof(maxStr);
                    cmd.durationMin = static_cast<int>(minSeconds * 1000);
                    cmd.durationMax = static_cast<int>(maxSeconds * 1000);
                    if (cmd.durationMin > cmd.durationMax) {
                        std::swap(cmd.durationMin, cmd.durationMax);
                    }
                    cmd.duration = cmd.durationMin;
                    cmd.useRandomRange = true;
                } catch (...) {
                    std::cerr << "Warning: Invalid pause duration at line " << lineNumber << ": " << line << std::endl;
                    continue;
                }
            } else {
                // Single value
                try {
                    float seconds = std::stof(durationStr);
                    cmd.duration = static_cast<int>(seconds * 1000);
                    cmd.durationMin = cmd.duration;
                    cmd.durationMax = cmd.duration;
                    cmd.useRandomRange = false;
                } catch (...) {
                    std::cerr << "Warning: Invalid pause duration at line " << lineNumber << ": " << line << std::endl;
                    continue;
                }
            }
            commands.push_back(cmd);
        }
        else if (commandType == "KEYDOWN" || commandType == "KEYPRESS") {
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << "Warning: Missing key at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            
            std::string durationStr;
            if (!(iss >> durationStr)) {
                std::cerr << "Warning: Invalid key duration at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            
            if (!parseValueOrRange(durationStr, cmd.duration, cmd.durationMin, cmd.durationMax, cmd.useRandomRange)) {
                std::cerr << "Warning: Invalid key duration at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            
            cmd.key = toUpper(cmd.key);
            commands.push_back(cmd);
        }
        else if (commandType == "KEY") {
            // Single key press (down and up immediately)
            iss >> cmd.key;
            if (cmd.key.empty()) {
                std::cerr << "Warning: Missing key at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            cmd.key = toUpper(cmd.key);
            cmd.duration = 50; // Default 50ms press
            cmd.durationMin = 50;
            cmd.durationMax = 50;
            cmd.useRandomRange = false;
            commands.push_back(cmd);
        }
        else if (commandType == "LOOP") {
            // Loop support - simple implementation
            int loopCount;
            if (!(iss >> loopCount)) {
                std::cerr << "Warning: Invalid loop count at line " << lineNumber << ": " << line << std::endl;
                continue;
            }
            cmd.duration = loopCount;
            cmd.useRandomRange = false;
            commands.push_back(cmd);
        }
        else if (commandType == "ENDLOOP") {
            commands.push_back(cmd);
        }
        else {
            std::cerr << "Warning: Unknown command at line " << lineNumber << ": " << line << std::endl;
        }
    }
    
    file.close();
    return commands;
}

// Simulate key press
void pressKey(BYTE vkCode, int duration) {
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
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
    
    // Send key up
    SendInput(1, &input[1], sizeof(INPUT));
}

// Execute the macro commands
void executeMacro(const std::vector<Command>& commands) {
    std::vector<int> loopStack;
    
    for (size_t i = 0; i < commands.size(); i++) {
        const Command& cmd = commands[i];
        
        // Check for abort key (ESC)
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) {
            std::cout << "Macro aborted by user (ESC pressed)" << std::endl;
            break;
        }
        
        if (cmd.type == "PAUSE") {
            int actualDuration = cmd.duration;
            if (cmd.useRandomRange) {
                actualDuration = getRandomInRange(cmd.durationMin, cmd.durationMax);
                std::cout << "Pausing for " << actualDuration / 1000.0 << " seconds (random from " 
                          << cmd.durationMin / 1000.0 << "-" << cmd.durationMax / 1000.0 << ")..." << std::endl;
            } else {
                std::cout << "Pausing for " << actualDuration / 1000.0 << " seconds..." << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(actualDuration));
        }
        else if (cmd.type == "KEYDOWN" || cmd.type == "KEYPRESS" || cmd.type == "KEY") {
            auto it = keyMap.find(cmd.key);
            if (it != keyMap.end()) {
                int actualDuration = cmd.duration;
                if (cmd.useRandomRange) {
                    actualDuration = getRandomInRange(cmd.durationMin, cmd.durationMax);
                    std::cout << "Pressing key: " << cmd.key << " for " << actualDuration << "ms (random from " 
                              << cmd.durationMin << "-" << cmd.durationMax << "ms)" << std::endl;
                } else {
                    std::cout << "Pressing key: " << cmd.key << " for " << actualDuration << "ms" << std::endl;
                }
                pressKey(it->second, actualDuration);
            }
            else {
                std::cerr << "Unknown key: " << cmd.key << std::endl;
            }
        }
        else if (cmd.type == "LOOP") {
            loopStack.push_back(cmd.duration);
            std::cout << "Starting loop (" << cmd.duration << " iterations)" << std::endl;
        }
        else if (cmd.type == "ENDLOOP") {
            if (!loopStack.empty()) {
                int& count = loopStack.back();
                count--;
                if (count > 0) {
                    // Find matching LOOP command
                    int loopLevel = 1;
                    for (int j = i - 1; j >= 0; j--) {
                        if (commands[j].type == "ENDLOOP") {
                            loopLevel++;
                        }
                        else if (commands[j].type == "LOOP") {
                            loopLevel--;
                            if (loopLevel == 0) {
                                i = j; // Set to the LOOP command position
                                break;
                            }
                        }
                    }
                }
                else {
                    loopStack.pop_back();
                    std::cout << "Loop completed" << std::endl;
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    std::string configFile = "macro.ini";
    
    // Check for command line argument
    if (argc > 1) {
        configFile = argv[1];
    }
    
    std::cout << "================================" << std::endl;
    std::cout << "Keyboard Macro Tool v1.2" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << "Loading config from: " << configFile << std::endl;
    std::cout << "Press ESC at any time to abort the macro" << std::endl;
    std::cout << std::endl;
    
    try {
        // Parse config file
        std::vector<Command> commands = parseConfig(configFile);
        
        if (commands.empty()) {
            std::cerr << "No valid commands found in config file" << std::endl;
            return 1;
        }
        
        std::cout << "Loaded " << commands.size() << " commands" << std::endl;
        std::cout << "Starting in 3 seconds..." << std::endl;
        std::cout << std::endl;
        
        // Give user time to switch to target application
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        // Execute macro
        executeMacro(commands);
        
        std::cout << std::endl;
        std::cout << "Macro execution completed!" << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
