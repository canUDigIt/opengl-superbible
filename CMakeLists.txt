cmake_minimum_required(VERSION 3.6)

project(openglsb VERSION 0.1 LANGUAGES C CXX)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw-3.2.1)

add_executable(openglsb
    src/main.cpp
    dependencies/glad/src/glad.c)

target_include_directories(openglsb
    PUBLIC
        dependencies/glad/include
        dependencies/glm-0.9.8.5
)

target_link_libraries(openglsb
    PRIVATE
        glfw

        OpenGL::OpenGL
        OpenGL::GLX
        OpenGL::GLU
)

target_compile_features(openglsb
    PUBLIC
        cxx_auto_type
        cxx_range_for

    PRIVATE
        cxx_variadic_templates
)
