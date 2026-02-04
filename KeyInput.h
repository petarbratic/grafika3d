#pragma once
#include "GLCommon.h"
#include "AppContext.h"

namespace KeyInput
{
    void applyKeyMapping(AppContext& ctx, bool pianoMode);
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
}
