cmake_minimum_required(VERSION 4.3)

project(simple_game LANGUAGES CXX)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

add_executable(simple_game ${SOURCES})

target_compile_definitions(
  "${CMAKE_PROJECT_NAME}"
  PUBLIC DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/"
)

target_include_directories(
  simple_game PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    ${SDL3_INCLUDE_DIRS}
    ${SDL3_IMAGE_INCLUDE_DIRS}
    ${SDL3_MIXER_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(
  simple_game PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    glm::glm-header-only
)
