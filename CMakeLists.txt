set(CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 3.20)
project(SIMM_ENGINE)

# 1. Global Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Whole Program Optimization (LTO) for performance in Release
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# 2. The Development Toggle
option(HOT_RELOAD "Build game as DLL for hot reloading" ON)

if(HOT_RELOAD)
	add_definitions(-DBUILD_HOT_RELOAD)
endif()

# 3. Add Subdirectories
add_subdirectory(source/common)
add_subdirectory(source/engine)
add_subdirectory(source/game)
add_subdirectory(source/launcher)

enable_testing()
add_subdirectory(source/engine_tests)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Set the default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Launcher)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")