cmake_minimum_required(VERSION 3.16...3.31)
project(wyrwyk LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default to Release for single-config generators (Makefiles, Ninja)
get_property(WYRWYK_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT WYRWYK_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif ()

option(WYRWYK_USE_SYSTEM_GLFW "Use GLFW installed on the system if it is available" ON)

include(FetchContent)

# GLFW - window, OpenGL context and input
if (WYRWYK_USE_SYSTEM_GLFW)
    find_package(glfw3 3.3 CONFIG QUIET)
endif ()
if (TARGET glfw)
    message(STATUS "Using system GLFW ${glfw3_VERSION}")
else ()
    message(STATUS "GLFW not found on the system, it will be downloaded and built from source")
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(glfw
            GIT_REPOSITORY https://github.com/glfw/glfw.git
            GIT_TAG 3.5.1
            GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(glfw)
endif ()

# Dear ImGui - GUI
FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.9b
        GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(imgui)

add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)
target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_link_libraries(imgui PUBLIC glfw)

# glad - OpenGL 3.3 core loader (generated, see contrib/glad)
add_library(glad STATIC contrib/glad/src/gl.c)
target_include_directories(glad PUBLIC contrib/glad/include)

# Executable
add_executable(wyrwyk
        src/main.cpp
        src/wyrwyk.cpp
        src/parser/parser.cpp
        src/parser/symbol-def.cpp
        src/parser/symbol-defs.cpp
        src/parser/tokenizer.cpp
        src/parser/translator.cpp
        src/parser/validator.cpp
        src/renderer/renderer.cpp
        src/utils/debug.cpp
        src/utils/load-file.cpp
        src/utils/resources.cpp)
target_include_directories(wyrwyk PRIVATE src contrib/stb)
target_link_libraries(wyrwyk PRIVATE glad glfw imgui)
target_compile_definitions(wyrwyk PRIVATE GLFW_INCLUDE_NONE $<$<CONFIG:Debug>:DEBUG>)
if (MSVC)
    target_compile_definitions(wyrwyk PRIVATE _CRT_SECURE_NO_WARNINGS)
endif ()

# Resources are looked up next to the executable
add_custom_command(TARGET wyrwyk POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res
        $<TARGET_FILE_DIR:wyrwyk>/res)

# Visual Studio: make wyrwyk the startup project
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT wyrwyk)
set_property(TARGET wyrwyk PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:wyrwyk>)

# Installation
install(TARGETS wyrwyk DESTINATION wyrwyk)
install(DIRECTORY res DESTINATION wyrwyk)
