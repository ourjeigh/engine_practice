include(FetchContent)

# 1. Download Google Test
FetchContent_Declare(
	googletest
	URL https://github.com/google/googletest/archive/refs/heads/main.zip
	DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# For Windows, we force GTest to use the same runtime as our engine
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Define the Test Executable
file(GLOB_RECURSE TEST_SOURCES *.cpp *.h)
add_executable(EngineTests ${TEST_SOURCES})

# 3. Linking and Includes
# We link against our Engine and the GTest libraries
target_link_libraries(EngineTests 
	PRIVATE 
		Common
		Engine 
		GTest::gtest_main
)

# 4. Standard C++ testing setup
include(GoogleTest)
gtest_discover_tests(EngineTests)

target_include_directories(EngineTests PRIVATE 
	${CMAKE_SOURCE_DIR}/source/engine
)

set_target_properties(gtest gtest_main gmock gmock_main PROPERTIES FOLDER "CMake")