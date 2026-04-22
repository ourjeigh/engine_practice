############
### GAME ###
############

file(GLOB_RECURSE GAME_SOURCES *.cpp *.h)

if(HOT_RELOAD)
	# Build as DLL for dev
	add_library(game SHARED ${GAME_SOURCES})
	target_compile_definitions(game PRIVATE GAME_EXPORT)
else()
	# Build as Static Lib for release
	add_library(game STATIC ${GAME_SOURCES})
endif()

# The game needs the common headers, but DOES NOT link against the Engine lib
target_include_directories(game 
	PRIVATE 
		${CMAKE_SOURCE_DIR}/src/common
)

target_link_libraries(game PRIVATE common)

target_compile_definitions(game PRIVATE GAME_SOURCE)