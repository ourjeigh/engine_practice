add_executable(Launcher main.cpp)

# Launcher ALWAYS needs the Engine
target_link_libraries(Launcher PRIVATE Engine)
target_include_directories(Launcher PRIVATE ${CMAKE_SOURCE_DIR}/source/common)
target_include_directories(Launcher PRIVATE ${CMAKE_SOURCE_DIR}/source/engine)

# Launcher ONLY links GameLayer if we aren't hot-reloading
if(NOT HOT_RELOAD)
    target_link_libraries(Launcher PRIVATE GameLayer)
endif()