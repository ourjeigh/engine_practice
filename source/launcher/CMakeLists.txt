################
### LAUNCHER ###
################

add_executable(launcher WIN32 main.cpp)

# launcher ALWAYS needs the engine
target_link_libraries(launcher PRIVATE engine)

target_include_directories(launcher
	PRIVATE 
		${CMAKE_SOURCE_DIR}/source/common
		${CMAKE_SOURCE_DIR}/source/engine)

if(HOT_RELOAD)
	# we still want it to build the game when we build launcher
	add_dependencies(launcher game)
else()
	# Launcher ONLY links game if we aren't hot-reloading
	target_link_libraries(launcher PRIVATE game)
endif()
