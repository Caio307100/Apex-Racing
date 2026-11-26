cmake_minimum_required(VERSION 3.20)
project(ApexRacingEngine VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)
find_package(OpenGL REQUIRED)

add_executable(apex-racing engine/main.cpp)
target_link_libraries(apex-racing PRIVATE PkgConfig::SDL2 OpenGL::GL)
target_compile_options(apex-racing PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
)
