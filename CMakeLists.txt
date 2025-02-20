cmake_minimum_required(VERSION 3.10)
project(TelemetryLib)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create the library
add_library(telemetry STATIC src/TelemetryLogger.cpp)

# Or use SHARED instead of STATIC if you want a dynamic library
# add_library(telemetry SHARED src/TelemetryLogger.cpp)

# Include public headers
target_include_directories(telemetry PUBLIC include)

# Optional: Set output directory for the library
set_target_properties(telemetry PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
