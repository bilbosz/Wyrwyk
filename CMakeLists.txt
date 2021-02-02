cmake_minimum_required(VERSION 3.17)
project(wyrwyk)

set(CMAKE_CXX_STANDARD 17)

add_compile_definitions(DEBUG)

include_directories(src)

include_directories(contrib/imgui)

add_executable(wyrwyk
        src/main.cpp
        src/utils/debug.cpp
        contrib/imgui/imgui.cpp
        contrib/imgui/imgui_draw.cpp
        contrib/imgui/imgui_tables.cpp
        contrib/imgui/imgui_widgets.cpp
        contrib/imgui/backends/imgui_impl_glfw.cpp
        contrib/imgui/backends/imgui_impl_opengl3.cpp
        src/wyrwyk.cpp src/parser/parser.cpp src/parser/symbol-def.cpp src/parser/symbol-defs.cpp src/parser/tokenizer.cpp src/parser/translator.cpp src/parser/validator.cpp src/renderer/renderer.cpp src/utils/load-file.cpp)

# External libraries
find_package(PkgConfig REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(wyrwyk ${OPENGL_LIBRARIES})

# GLFW
pkg_search_module(GLFW REQUIRED glfw3)
include_directories(${GLFW_INCLUDE_DIRS})
target_link_libraries(wyrwyk ${GLFW_LIBRARIES})

# GLEW
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(wyrwyk ${GLEW_LIBRARIES})