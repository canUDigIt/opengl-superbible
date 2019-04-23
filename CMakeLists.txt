cmake_minimum_required(VERSION 3.11)

project(openglsb VERSION 0.1 LANGUAGES C CXX)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/dependencies")

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/application.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        glfw
        glad
        glm

        OpenGL::OpenGL
        OpenGL::GLX
        OpenGL::GLU
)

target_compile_features(${PROJECT_NAME}
    PRIVATE
        cxx_auto_type
        cxx_range_for
        cxx_variadic_templates
)
