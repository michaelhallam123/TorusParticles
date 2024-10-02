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
    jsoncpp_lib
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    )

FetchContent_MakeAvailable(glfw glad jsoncpp_lib)

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
    jsoncpp
    )