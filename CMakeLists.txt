cmake_minimum_required(VERSION 3.16)
project(TheoryGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Enable Qt MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/code)

# Collect all source files explicitly
set(SOURCES
    TheoryGUI_single.cpp
    code/automaton/automaton.cpp
    code/lexer/lexer.cpp
    code/parser/parser.cpp
    code/semantic/semantic.cpp
)

# Create executable
add_executable(TheoryGUI ${SOURCES})

# Link Qt6 libraries
target_link_libraries(TheoryGUI Qt6::Core Qt6::Widgets)

# Set Windows subsystem (optional)
if(WIN32)
    set_target_properties(TheoryGUI PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
