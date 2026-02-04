#pragma once

#include "GLCommon.h"

class InputToggles
{
public:
    InputToggles();

    void update(GLFWwindow* window);

    bool depthEnabled() const { return m_depthEnabled; }
    bool cullEnabled() const { return m_cullEnabled; }

private:
    bool m_depthEnabled = true;
    bool m_cullEnabled = false;

    bool m_insertWasDown = false;
    bool m_deleteWasDown = false;
};
