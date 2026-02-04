#pragma once

#include <string>
#include "GLCommon.h"

class OverlayRenderer
{
public:
    OverlayRenderer() = default;
    ~OverlayRenderer() = default;

    // default: bottom-right, bez margine, w = 0.35, h iz aspect-a (117/230)
    bool init(const char* texturePath, float w = 0.35f, float aspectH_over_W = (117.0f / 230.0f));
    void shutdown();

    void draw() const;

    bool isReady() const { return m_tex != 0 && m_program != 0 && m_vao != 0; }

private:
    GLuint m_program = 0;
    GLuint m_tex = 0;
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
};
