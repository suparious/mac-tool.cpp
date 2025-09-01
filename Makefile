# Makefile for Keyboard Macro Tool

CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -static
LDFLAGS = -luser32
TARGET = macro_tool.exe
SOURCE = macro_tool.cpp

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE) $(LDFLAGS)

# Clean build artifacts
clean:
	del $(TARGET) 2>nul || true

# Run with default config
run: $(TARGET)
	./$(TARGET)

# Run with custom config
run-custom: $(TARGET)
	./$(TARGET) custom_macro.ini

# Run test scenarios
test: $(TARGET)
	./$(TARGET) test_scenarios.ini

# Show help
help:
	@echo Available targets:
	@echo   all         - Build the macro tool (default)
	@echo   clean       - Remove built executable
	@echo   run         - Build and run with default config
	@echo   run-custom  - Build and run with custom_macro.ini
	@echo   test        - Build and run test scenarios
	@echo   help        - Show this help message

.PHONY: all clean run run-custom test help
