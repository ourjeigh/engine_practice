##############
### ENGINE ###
##############

file(GLOB_RECURSE ENGINE_SOURCES *.cpp *.h)

add_library(engine STATIC ${ENGINE_SOURCES})

if(WIN32)
	target_link_libraries(engine PRIVATE winmm.lib)
endif()

target_include_directories(engine 
	PRIVATE
		.
)

target_link_libraries(engine PRIVATE common)

target_compile_definitions(engine PRIVATE ENGINE_SOURCE)