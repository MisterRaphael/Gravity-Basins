cmake_minimum_required(VERSION 3.20) # Version plus standard
project(Gravity_Basins C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimisations pour la performance pure
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -ffast-math -march=native")

# Les compute shaders et les SSBO utilisés par la simulation GPU nécessitent OpenGL 4.3.
set(OPENGL_VERSION "4.3" CACHE STRING "OpenGL version used by raylib" FORCE)

set(SRC_FILES
        src/main.c
        src/window.c
        src/draw.c
        src/gpu_simulation.c
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

target_link_libraries(Gravity_Basins PRIVATE raylib m)

# Les shaders sont accessibles depuis le répertoire de build, qui est le répertoire
# de travail utilisé par les configurations de lancement CMake. Cette cible est exécutée
# à chaque build afin de prendre en compte les modifications GLSL sans relinker le C.
add_custom_target(copy_gravity_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:Gravity_Basins>/resources/shaders"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_compute.glsl"
                "$<TARGET_FILE_DIR:Gravity_Basins>/resources/shaders/gravity_compute.glsl"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_render.vs"
                "$<TARGET_FILE_DIR:Gravity_Basins>/resources/shaders/gravity_render.vs"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_render.fs"
                "$<TARGET_FILE_DIR:Gravity_Basins>/resources/shaders/gravity_render.fs"
        DEPENDS
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_compute.glsl"
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_render.vs"
                "${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/gravity_render.fs"
)
add_dependencies(Gravity_Basins copy_gravity_shaders)

