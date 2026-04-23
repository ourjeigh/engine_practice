############
### GAME ###
############

file(GLOB_RECURSE GAME_SOURCES *.cpp *.h)

if(HOT_RELOAD)
	# Build as DLL for dev
	add_library(game SHARED ${GAME_SOURCES})
	target_compile_definitions(game PRIVATE GAME_EXPORT)
	
	# 1. Inject the BuildTimestamp property into the .vcxproj
	# This will evaluate inside Visual Studio EVERY time you build.
	set_target_properties(game PROPERTIES 
		VS_GLOBAL_BuildTimestamp "$([System.DateTime]::Now.ToString(\"yyyyMMdd_HHmmss\"))"
	)

	# 2. Use that property in the Linker flags
	# We use the $(BuildTimestamp) syntax, which is MSBuild's way of 
	# accessing the variable we just created.
	target_link_options(game PRIVATE 
		"/PDB:$(OutDir)game_debug_win64_$(BuildTimestamp).pdb"
		"/PDBALTPATH:game_debug_win64_$(BuildTimestamp).pdb")

	set_target_properties(game PROPERTIES LINK_FLAGS "/INCREMENTAL:NO")
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