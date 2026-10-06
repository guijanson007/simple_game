cmake_minimum_required(VERSION 4.3)

project(simple_game LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

# This is for wayland support on Linux
set(SDL_X11 OFF CACHE BOOL "" FORCE)

# Disable image formats we don't need to reduce build time and dependencies
set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)


# find_package(SDL3 REQUIRED)
# find_package(SDL3_image REQUIRED)
# find_package(SDL3_mixer REQUIRED)
# find_package(glm REQUIRED)

include(FetchContent)

# Pre-configure options before making them available.
# This prevents the dependencies from building their own tests/examples,
# which drastically reduces your compilation time.
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF CACHE BOOL "" FORCE)


FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        main
)

FetchContent_Declare(
    SDL3_image
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
    GIT_TAG        main
)

FetchContent_Declare(
    SDL3_mixer
    GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer.git
    GIT_TAG        main
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        1.0.1
)

# Downloads the repos (if not already downloaded) and calls add_subdirectory on them
FetchContent_MakeAvailable(SDL3 SDL3_image SDL3_mixer glm)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(simple_game ${SOURCES})

target_compile_definitions(
  "${CMAKE_PROJECT_NAME}"
  PUBLIC DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

target_include_directories(
  simple_game PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

target_link_libraries(
  simple_game PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    glm::glm
)
