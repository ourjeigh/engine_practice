
# 1. Collect all headers and source files recursively
file(GLOB_RECURSE COMMON_FILES 
    "*.h" 
    "*.hpp" 
    "*.cpp" 
    "*.inl"
)

# If we start including cpp files in common, this will need to become STATIC
add_library(Common INTERFACE)

# 2. Tell CMake to mimic the disk structure in Visual Studio
# ${CMAKE_CURRENT_SOURCE_DIR} is the 'root' it uses to calculate the relative paths
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_FILES})

# 3. Add them to the target so they appear in the IDE
target_sources(Common INTERFACE ${COMMON_FILES})

# Set the include directory for anyone who links to Common
target_include_directories(Common INTERFACE .)

# 3. Create a CUSTOM TARGET specifically for the Visual Studio UI
# This forces "Common" to show up as its own project
add_custom_target(CommonProject SOURCES ${COMMON_FILES})

# 4. (Optional) Rename the custom target in VS so it just says "Common"
set_target_properties(CommonProject PROPERTIES PROJECT_LABEL "Common")

# 5. Apply the folder structure to the Custom Target
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${COMMON_FILES})

# Organize it into its own folder in VS
set_target_properties(Common PROPERTIES FOLDER "Common")