cmake_minimum_required(VERSION 3.20) # Version plus standard
project(Gravity_Basins C)

set(CMAKE_C_STANDARD 23)

# Optimisations pour la performance pure
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -ffast-math -march=native")

set(SRC_FILES
        src/main.c
        src/window.c
        src/draw.c
        src/calculus.c
        src/corpus_creation.c
        src/config.c
)

add_executable(Gravity_Basins ${SRC_FILES})
target_include_directories(Gravity_Basins PRIVATE include)

# Raylib
# Raylib (téléchargement et compilation automatiques)
include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        master
)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib)

target_link_libraries(Gravity_Basins PRIVATE raylib)

