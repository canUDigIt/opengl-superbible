cmake_minimum_required(VERSION 3.6)

project(openglsb VERSION 0.1 LANGUAGES C CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.2 REQUIRED)

add_executable(openglsb 
    src/main.cpp
    ${CMAKE_SOURCE_DIR}/dependencies/glad/src/glad.c)

target_include_directories(openglsb
    PUBLIC ${CMAKE_SOURCE_DIR}/dependencies/glad/include
    PRIVATE
        src
        include)

target_link_libraries(openglsb
    PUBLIC 
        glfw 
        dl)

if(OPENGL_FOUND)
    target_link_libraries(openglsb 
        PUBLIC ${OPENGL_gl_LIBRARY})
endif()

target_compile_features(openglsb
    PUBLIC
        cxx_auto_type 
        cxx_range_for
    PRIVATE
        cxx_variadic_templates)
