cmake_minimum_required(VERSION 4.3)

project(simple_game)

find_package(SDL3 REQUIRED)
find_package(SDL3_image REQUIRED)
find_package(SDL3_mixer REQUIRED)
find_package(glm REQUIRED)

add_executable(
  simple_game
  main.cpp
  SDL.cpp
  resources.cpp
  game.cpp
  map.cpp
)

if(CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET simple_game PROPERTY CXX_STANDARD 20)
endif()

set_target_properties(simple_game PROPERTIES
CXX_STANDARD 20
CXX_STANDARD_REQUIRED ON
CXX_EXTENSIONS OFF
)

# Help the program find the data files (images, sounds, etc.) at runtime
add_custom_command(TARGET simple_game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:simple_game>/data)

set_target_properties(
  simple_game PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/data")

target_link_libraries(
  simple_game PRIVATE
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_mixer::SDL3_mixer
    glm::glm-header-only
)
