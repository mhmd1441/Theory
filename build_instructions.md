# Theory of Computation GUI - Build Instructions

## Overview
This GUI application provides a graphical interface for the Theory of Computation project, including automaton management and program analysis features.

## Features
- **Automaton Management**: Add, delete, search, and test finite automata with form-based input
- **Program Analysis**: Tokenize, parse, and perform semantic analysis on pseudo-C code
- **Code Generation**: Convert pseudo-C to pseudo-Python
- **Visual Interface**: User-friendly Qt-based GUI with tabbed interface
- **No Console Input**: All operations use GUI forms instead of command-line prompts

## Prerequisites
- Qt6 (Core and Widgets modules)
- CMake 3.16 or higher
- C++17 compatible compiler

## Installation

### On macOS
```bash
# Install Qt6 using Homebrew
brew install qt@6

# Install CMake
brew install cmake
```

### On Windows
1. Download and install Qt6 from https://www.qt.io/download
2. Download and install CMake from https://cmake.org/download/
3. Make sure Qt6 and CMake are in your PATH

### On Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install qt6-base-dev cmake build-essential
```

## Building the Application

### Method 1: Using CMake (Recommended)
```bash
# Create build directory
mkdir build
cd build

# Configure the project
cmake ..

# Build the application
cmake --build .

# Run the application
./TheoryGUI
```

### Method 2: Using qmake (if you prefer)
```bash
# Generate Makefile
qmake TheoryGUI.pro

# Build
make

# Run
./TheoryGUI
```

## Usage

### Automaton Management Tab
1. **Add Automaton**: Fill in the automaton details in the form:
   - **Alphabet**: Enter symbols without separators (e.g., `ab`)
   - **States**: Enter comma-separated states (e.g., `q0,q1,q2`)
   - **Initial State**: Enter the initial state (e.g., `q0`)
   - **Transitions**: Enter comma-separated transitions (e.g., `q0,a,q1;q1,b,q2`)
   - **Final States**: Enter comma-separated final states (e.g., `q2`)
   Click "Add Automaton" to create it.

2. **Delete Automaton**: Click "Delete" and enter the automaton ID in the dialog

3. **Search Automaton**: Click "Search" and enter the automaton ID to find it

4. **Test Automaton**: 
   - Enter an input string in the test field
   - Click "Test" and enter the automaton ID
   - Results show ACCEPTED/REJECTED

### Program Analysis Tab
1. **Enter Program**: Type or paste pseudo-C code in the input area
2. **Load Example**: Click "Load Example" to see a sample program
3. **Analyze**: Click "Analyze Program" to perform:
   - Lexical analysis (tokenization)
   - Parsing (syntax analysis)
   - Semantic analysis (variable checking)
   - Code generation (to Python)
4. **View Results**: Switch between tabs to see tokens, parse tree, semantic analysis, and generated code

## Data Format Examples

### Automaton Example
- **Alphabet**: `ab`
- **States**: `q0,q1,q2`
- **Initial State**: `q0`
- **Transitions**: `q0,a,q1;q1,b,q2;q2,a,q0`
- **Final States**: `q2`

### Program Example
```c
int x = 10;
if (x > 5) {
    int y = x * 2;
    return y;
} else {
    return 0;
}
```

## File Structure
```
Theory/
├── TheoryGUI_single.cpp    # Main GUI application (fixed version)
├── CMakeLists.txt          # CMake build configuration
├── build_instructions.md   # This file
├── TheoryFinalProject.cpp  # Original console application
└── code/                   # Backend modules
    ├── automaton/
    ├── lexer/
    ├── parser/
    └── semantic/
```

## Troubleshooting

### Qt6 Not Found
If CMake can't find Qt6, set the Qt6 path:
```bash
cmake -DQt6_DIR=/path/to/qt6/lib/cmake/Qt6 ..
```

### Build Errors
- Ensure all backend source files are present in the `code/` directory
- Check that Qt6 Core and Widgets modules are properly installed
- Verify C++17 support is enabled

### Runtime Issues
- Make sure all required Qt6 libraries are available
- Check that the backend modules are properly linked

### GUI Issues Fixed
- **Console Input Problem**: The original issue where adding automata waited for console input has been fixed
- **Form-Based Input**: All automaton operations now use GUI forms instead of command-line prompts
- **Error Handling**: User-friendly error messages and validation

## Notes
- The GUI wraps the existing console functionality
- All original backend functions are preserved
- The application automatically cleans up memory on exit
- Error handling provides user-friendly messages for common issues
- No more console input prompts - everything works through the GUI interface
