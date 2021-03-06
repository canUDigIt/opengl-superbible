#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class application
{
public:
    application() : mWidth(800), mHeight(600) {}
    virtual ~application() {}

    // Public interface
    virtual void render(double currentTime) = 0;
    virtual void init() {}
    virtual void startup() {}
    virtual void shutdown() {}
    virtual void resize(int width, int height) {}
    virtual void onKeyPress(int key, int action) {}
    virtual void onMouseButton(int button, int action) {}
    virtual void onMouseMove(int x, int y) {}
    virtual void onMouseWheel(double offset) {}

    int run() {
        // Load GLFW and Create a Window
        glfwInit();

        init();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

        if (mWindow == nullptr) {
            std::cerr << "Failed to create a GLFW window\n";
            return EXIT_FAILURE;
        }

        // Create Context and Load OpenGL Functions
        glfwMakeContextCurrent(mWindow);
        if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
            std::cerr << "Failed to initialize OpenGL context\n";
        }

        std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";

        // Setting up callbacks
        //
        // We need this call so that we can actuall call our callbacks
        glfwSetWindowUserPointer(mWindow, this);

        auto resizeCallback = [](GLFWwindow* window, int w, int h){
            auto self = static_cast<application*>(glfwGetWindowUserPointer(window));
            self->resize(w, h);
        };
        auto keyCallback = [](GLFWwindow* window, int key, int scancode, int action, int mods){
            auto self = static_cast<application*>(glfwGetWindowUserPointer(window));
            self->onKeyPress(key, action);
        };
        auto mouseButtonCallback = [](GLFWwindow* window, int button, int action, int mods){
            auto self = static_cast<application*>(glfwGetWindowUserPointer(window));
            self->onMouseButton(button, action);
        };
        auto mouseMoveCallback = [](GLFWwindow* window, double x, double y){
            auto self = static_cast<application*>(glfwGetWindowUserPointer(window));
            self->onMouseMove(static_cast<int>(x), static_cast<int>(y));
        };
        auto mouseWheelCallback = [](GLFWwindow* window, double xoffset, double yoffset){
            auto self = static_cast<application*>(glfwGetWindowUserPointer(window));
            self->onMouseWheel(yoffset);
        };

        glfwSetWindowSizeCallback(mWindow, resizeCallback);
        glfwSetKeyCallback(mWindow, keyCallback);
        glfwSetMouseButtonCallback(mWindow, mouseButtonCallback);
        glfwSetCursorPosCallback(mWindow, mouseMoveCallback);
        glfwSetScrollCallback(mWindow, mouseWheelCallback);

        // Do whatever startup you need here
        startup();

        // Rendering Loop
        while (glfwWindowShouldClose(mWindow) == false) {
            if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
                glfwSetWindowShouldClose(mWindow, true);

            render(glfwGetTime());

            // Flip Buffers and Draw
            glfwSwapBuffers(mWindow);
            glfwPollEvents();
        }

        shutdown();

        glfwDestroyWindow(mWindow);
        glfwTerminate();
        return EXIT_SUCCESS;
    }

protected:
    unsigned int mWidth;
    unsigned int mHeight;
};
