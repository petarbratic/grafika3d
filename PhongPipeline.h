#pragma once

#include "GLCommon.h"

class PhongPipeline
{
public:
    PhongPipeline() = default;
    ~PhongPipeline() = default;

    bool init(int viewportW, int viewportH);
    void shutdown();

    void use() const;
    void stop() const;

    GLuint program() const { return m_program; }
    int modelLoc() const { return m_modelLoc; }

private:
    GLuint m_program = 0;

    int m_modelLoc = -1;
    int m_viewLoc = -1;
    int m_projLoc = -1;

    int m_viewPosLoc = -1;

    int m_lightPosLoc = -1;
    int m_lightALoc = -1;
    int m_lightDLoc = -1;
    int m_lightSLoc = -1;

    int m_matShineLoc = -1;
    int m_matALoc = -1;
    int m_matDLoc = -1;
    int m_matSLoc = -1;
};
