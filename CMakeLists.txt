cmake_minimum_required(VERSION 3.28.1)
project(Mango
        VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(glfw
        GIT_REPOSITORY  https://github.com/glfw/glfw.git
        GIT_TAG         3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.2
)
FetchContent_MakeAvailable(glm)

# EnTT
FetchContent_Declare(entt
        GIT_REPOSITORY https://github.com/skypjack/entt
        GIT_TAG        v3.16.0
)
FetchContent_MakeAvailable(entt)

# FreeType
FetchContent_Declare(freetype
        GIT_REPOSITORY https://github.com/freetype/freetype.git
        GIT_TAG        VER-2-14-2
)
set(FT_WITH_ZLIB     OFF CACHE BOOL "" FORCE)
set(FT_WITH_BZIP2    OFF CACHE BOOL "" FORCE)
set(FT_WITH_PNG      OFF CACHE BOOL "" FORCE)
set(FT_WITH_HARFBUZZ OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(freetype)

add_library(project_options INTERFACE)
target_compile_definitions(project_options
        INTERFACE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
)

add_subdirectory(Engine)
add_subdirectory(Contents)