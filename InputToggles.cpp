#include "GLCommon.h"
#include "InputToggles.h"

InputToggles::InputToggles()
{
    m_depthEnabled = true;
    m_cullEnabled = false;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
}

void InputToggles::update(GLFWwindow* window)
{
    // TOGGLE: DEPTH TEST (INSERT)
    const bool insertDown = glfwGetKey(window, GLFW_KEY_INSERT) == GLFW_PRESS;
    if (insertDown && !m_insertWasDown)
    {
        m_depthEnabled = !m_depthEnabled;
        if (m_depthEnabled) glEnable(GL_DEPTH_TEST);
        else glDisable(GL_DEPTH_TEST);
    }
    m_insertWasDown = insertDown;

    // TOGGLE: FACE CULLING (DELETE)
    const bool deleteDown = glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS;
    if (deleteDown && !m_deleteWasDown)
    {
        m_cullEnabled = !m_cullEnabled;
        if (m_cullEnabled)
        {
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }
    }
    m_deleteWasDown = deleteDown;
}
