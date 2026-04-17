file(GLOB_RECURSE ENGINE_SOURCES *.cpp *.h)

add_library(Engine STATIC ${ENGINE_SOURCES})

if(WIN32)
	target_link_libraries(Engine PRIVATE winmm.lib)
endif()

target_include_directories(Engine 
	PRIVATE
		.
)

target_link_libraries(Engine PRIVATE Common)

target_compile_definitions(Engine PRIVATE ENGINE_SOURCE)
target_compile_definitions(Engine PRIVATE -DGAME_SOURCE)