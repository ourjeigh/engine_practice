##############
### ENGINE ###
##############

file(GLOB_RECURSE ENGINE_SOURCES 
    *.h
    *.cpp
    *.inl)

add_library(engine STATIC ${ENGINE_SOURCES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${ENGINE_SOURCES})
target_sources(engine PRIVATE ${ENGINE_SOURCES})

target_include_directories(engine 
	PRIVATE
		.
)

target_link_libraries(engine PRIVATE common)

if(WIN32)
	target_link_libraries(engine PRIVATE winmm.lib)
endif()

target_compile_definitions(engine PRIVATE ENGINE_SOURCE)

add_custom_command(
	TARGET engine
	PRE_BUILD
	COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_SOURCE_DIR}/build_todo.ps1"
	VERBATIM
)