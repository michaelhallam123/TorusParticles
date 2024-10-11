cmake_minimum_required(VERSION 3.24)

project(TorusParticles VERSION 1.0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)

# Fetch GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
)

# Fetch GLAD
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
)
set(GLAD_PROFILE "core"	CACHE STRING "OpenGL profile")
set(GLAD_API "gl=3.3" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest")

# Fetch jsoncpp
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
)
set(JSONCPP_WITH_TESTS OFF CACHE INTERNAL "Compile and (for jsoncpp_check) run JsonCpp test executables")
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE INTERNAL "Automatically run unit-tests as a post build step")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build jsoncpp_lib as a shared library.")
set(BUILD_OBJECT_LIBS OFF CACHE INTERNAL "Build jsoncpp_lib as a object library.")

FetchContent_MakeAvailable(glfw glad jsoncpp)

# Find OpenGL
find_package(OpenGL REQUIRED)

add_executable(
    TorusParticles
    
    "src/main.cpp" 

    "src/graphics/Renderer.cpp" "src/graphics/Renderer.hpp"
    "src/graphics/Shader.cpp" "src/graphics/Shader.hpp"
    "src/graphics/Window.cpp" "src/graphics/Window.hpp"

    "src/physics/SpatialHashSolver/Cell.hpp"
    "src/physics/SpatialHashSolver/SpatialHashSolver.cpp" 
    "src/physics/SpatialHashSolver/SpatialHashSolver.hpp"

    "src/physics/Ball.hpp"
    "src/physics/BallType.hpp"
    "src/physics/Solver.cpp" "src/physics/Solver.hpp"
    "src/physics/Vec2.hpp"
    "src/physics/World.hpp" 

    "src/utils/loadPreset.cpp" "src/utils/loadPreset.hpp"
    "src/utils/Preset.hpp"
)

target_include_directories(
    TorusParticles PRIVATE 
    "src/graphics" 
    "src/physics" "src/physics/SpatialHashSolver"
    "src/utils"
)

target_link_libraries(TorusParticles
    PRIVATE OpenGL::GL 
    PRIVATE glfw 
    PRIVATE glad
    PRIVATE jsoncpp_static
    )
 
# Move shaders and presets to binary location
add_custom_command(
    TARGET TorusParticles
    COMMENT "Copy presets directory"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/presets $<TARGET_FILE_DIR:${PROJECT_NAME}>
    VERBATIM)

add_custom_command(
    TARGET TorusParticles
    COMMENT "Copy shaders directory"
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/src/graphics/shaders $<TARGET_FILE_DIR:${PROJECT_NAME}>/shaders
    VERBATIM)