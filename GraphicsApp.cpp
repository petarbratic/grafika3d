#include "GLCommon.h"
#include "GraphicsApp.h"

#include <iostream>
#include <thread>
#include <chrono>

bool GraphicsApp::initFullscreen(const char* title)
{
    if (!glfwInit()) {
        std::cout << "GLFW nije ucitan\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    m_width = mode->width;
    m_height = mode->height;

    m_window = glfwCreateWindow(m_width, m_height, title, monitor, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0); // VSync off

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW greska\n";
        shutdown();
        return false;
    }

    glfwSetFramebufferSizeCallback(m_window, GraphicsApp::framebufferSizeCallback);
    glViewport(0, 0, m_width, m_height);

    // Globalni state (ostaje kao kod tebe)
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE);

    return true;
}

void GraphicsApp::shutdown()
{
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
}

bool GraphicsApp::shouldClose() const
{
    return m_window == nullptr || glfwWindowShouldClose(m_window);
}

void GraphicsApp::setClearColor(float r, float g, float b, float a)
{
    m_clearR = r; m_clearG = g; m_clearB = b; m_clearA = a;
    glClearColor(m_clearR, m_clearG, m_clearB, m_clearA);
}

void GraphicsApp::beginFrame()
{
    glClearColor(m_clearR, m_clearG, m_clearB, m_clearA);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GraphicsApp::endFrame()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

double GraphicsApp::timeNow() const
{
    return glfwGetTime();
}

void GraphicsApp::limitFps(double targetFps, double frameStartTime)
{
    if (targetFps <= 0.0) return;

    const double targetFrameTime = 1.0 / targetFps;
    const double frameEnd = glfwGetTime();
    const double frameTime = frameEnd - frameStartTime;

    if (frameTime < targetFrameTime) {
        const double sleepSec = targetFrameTime - frameTime;
        if (sleepSec > 0.0) {
            std::this_thread::sleep_for(
                std::chrono::duration_cast<std::chrono::microseconds>(
                    std::chrono::duration<double>(sleepSec)
                )
            );
        }
    }
}

void GraphicsApp::framebufferSizeCallback(GLFWwindow*, int w, int h)
{
    glViewport(0, 0, w, h);
}
