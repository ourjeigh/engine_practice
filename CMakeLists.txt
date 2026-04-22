############
### ROOT ###
############

set(CMAKE_CONFIGURATION_TYPES "Debug;Dev;Release" CACHE STRING "" FORCE)
cmake_minimum_required(VERSION 3.20)
project(SIMM_ENGINE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

if(WIN32)
	set(PLAT_SUFFIX "win64")
# else()
#    set(PLAT_SUFFIX "unix") 
endif()

# Apply the config and platform naming convention globally
# This turns "game" into "game_debug_x64.dll" automatically
set(CMAKE_DEBUG_POSTFIX "_debug_${PLAT_SUFFIX}")
set(CMAKE_RELEASE_POSTFIX "_release_${PLAT_SUFFIX}")
set(CMAKE_DEV_POSTFIX "_dev_${PLAT_SUFFIX}")

# --- 2. Flattening the Directory Structure ---
# This loop tells CMake: "For every config (Debug, Release, etc.), 
# use exactly the /build folder, no subdirectories."
foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${CONFIG} CONFIG_UPPER)
	
	# Executables (.exe)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/build")
	# DLLs and Import Libs (.dll / .lib)
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/build")
	# Static Libs (.lib)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/build")
	# PDB files
	set(CMAKE_PDB_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/build")
endforeach()

# 1. Global Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Whole Program Optimization (LTO) for performance in Release
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)

# 2. The Development Toggle
option(HOT_RELOAD "Build game as DLL for hot reloading" ON)

if(HOT_RELOAD)
	add_compile_definitions(HOT_RELOAD)
	message("HOT_RELOAD enabled")
endif()

# 3. Add Subdirectories
add_subdirectory(source/common)
add_subdirectory(source/engine)
add_subdirectory(source/game)
add_subdirectory(source/launcher)

enable_testing()
add_subdirectory(source/engine_tests)


# Set the default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT launcher)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# explicitly set the working directory since we're overriding defaults
set_target_properties(launcher PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

# Force the postfix onto EVERYTHING (including Executables)
set_target_properties(launcher engine_tests PROPERTIES 
	DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
	RELEASE_POSTFIX "${CMAKE_RELEASE_POSTFIX}"
	DEV_POSTFIX "${CMAKE_DEV_POSTFIX}"
)