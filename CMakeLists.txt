cmake_minimum_required(VERSION 3.10)
project(dawieplusplus VERSION 1.0)

# Set C++ standard (adjust if needed, e.g., C++11/14/17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Place binaries in the 'bin' folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Source files (excluding tempCodeRunnerFile.c)
file(GLOB SOURCES "src/*.cpp")
set(HEADERS
    src/midifuncs.h
)

# Create executable
add_executable(dawieplusplus ${SOURCES})

# Optional: Add compiler warnings (adjust for your compiler)
if(MSVC)
    target_compile_options(dawieplusplus PRIVATE /W4)
else()
    target_compile_options(dawieplusplus PRIVATE -Wall -Wextra)
endif()

# Optional: If you need MIDI library (e.g., RtMidi)
# find_package(RtMidi REQUIRED)
# target_link_libraries(dawieplusplus RtMidi)