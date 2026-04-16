file(GLOB_RECURSE GAME_SOURCES *.cpp *.h)

if(HOT_RELOAD)
    # Build as DLL for dev
    add_library(Game SHARED ${GAME_SOURCES})
    target_compile_definitions(Game PRIVATE GAME_EXPORT)
else()
    # Build as Static Lib for release
    add_library(Game STATIC ${GAME_SOURCES})
endif()

# The Game needs the Common headers, but DOES NOT link against the Engine lib
target_include_directories(Game PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/common
)