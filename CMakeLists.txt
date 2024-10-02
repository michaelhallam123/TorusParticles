cmake_minimum_required(VERSION 3.24)

project(TorusParticles)

include(FetchContent)

# Find GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    FIND_PACKAGE_ARGS NAMES glfw3
    )

# Find GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    )
set(GLAD_PROFILE "core"	CACHE STRING "OpenGL profile")
set(GLAD_GENERATOR "c"	CACHE STRING "Language to generate the binding for")

# Download jsoncpp
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    )

set(JSONCPP_WITH_TESTS OFF CACHE INTERNAL "Compile and (for jsoncpp_check) run JsonCpp test executables")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE INTERNAL "Automatically run unit-tests as a post build step")

FetchContent_MakeAvailable(glfw glad jsoncpp)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Add source files
file(GLOB_RECURSE source_files
    "src/*.cpp"
    )

add_executable(TorusParticles ${source_files})
target_include_directories(TorusParticles PRIVATE "src/graphics" "src/physics" "src/utils")
target_link_libraries(TorusParticles
    OpenGL::GL 
    glfw 
    glad
    jsoncpp_static
    )

# Move shaders and presets to binary location
add_custom_command(
    TARGET TorusParticles
    COMMENT "Copy presets directory"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/presets $<TARGET_FILE_DIR:${PROJECT_NAME}>/presets
    VERBATIM)

add_custom_command(
    TARGET TorusParticles
    COMMENT "Copy shaders directory"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/graphics/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    VERBATIM)