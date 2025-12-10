cmake_minimum_required(VERSION 3.14)

project(CHIP8_Emulator C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

message(STATUS "Descargando SDL2... esto puede tardar un poco la primera vez.")

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
  GIT_TAG        release-2.30.0 
)

FetchContent_MakeAvailable(SDL2)

set(SOURCES
    src/main.c
    src/chip8.c
)

add_executable(chip8 ${SOURCES})

target_include_directories(chip8 PRIVATE 
    include 
    ${SDL2_SOURCE_DIR}/include 
)


#SDL2::SDL2main y SDL2::SDL2
target_link_libraries(chip8 PRIVATE SDL2::SDL2main SDL2::SDL2)

if(WIN32)
    add_custom_command(TARGET chip8 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2>
        $<TARGET_FILE_DIR:chip8>
    )
endif()
