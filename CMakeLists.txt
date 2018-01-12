cmake_minimum_required(VERSION 3.6)

project(openglsb VERSION 0.1 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(dependencies/glfw-3.2.1)

add_executable(openglsb
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c)

target_include_directories(openglsb
    PUBLIC
        ${CMAKE_SOURCE_DIR}/dependencies/glad/include
        ${CMAKE_SOURCE_DIR}/dependencies/glm-0.9.8.5
        ${CMAKE_SOURCE_DIR}/dependencies/gli
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(openglsb
    PRIVATE
        glfw)

if(OPENGL_FOUND)
    target_include_directories(openglsb PRIVATE ${OPENGL_INCLUDE_DIR})
    target_link_libraries(openglsb PRIVATE ${OPENGL_gl_LIBRARY})
endif()

target_compile_features(openglsb
    PUBLIC
        cxx_auto_type
        cxx_range_for
    PRIVATE
        cxx_variadic_templates)
