#pragma once

#include <string>

#include "GLCommon.h"

class GraphicsApp
{
public:
    GraphicsApp() = default;
    ~GraphicsApp() = default;

    bool initFullscreen(const char* title);
    void shutdown();

    GLFWwindow* window() const { return m_window; }
    int width() const { return m_width; }
    int height() const { return m_height; }

    bool shouldClose() const;
    void beginFrame();
    void endFrame();

    double timeNow() const;

    // FPS limiter: prosledi 'frameStartTime' (vreme na pocetku frame-a)
    void limitFps(double targetFps, double frameStartTime);

    void setClearColor(float r, float g, float b, float a);

private:
    static void framebufferSizeCallback(GLFWwindow* window, int w, int h);

private:
    GLFWwindow* m_window = nullptr;
    int m_width = 0;
    int m_height = 0;

    float m_clearR = 0.5f;
    float m_clearG = 0.5f;
    float m_clearB = 0.5f;
    float m_clearA = 1.0f;
};
