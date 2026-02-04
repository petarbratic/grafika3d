#pragma once
#include "GLCommon.h"

class AppContext;
class AccordionScene;

namespace AppSetup
{
    bool initAudio(AppContext& ctx);
    void shutdownAudio();

    void setupContextAndScene(AppContext& ctx, AccordionScene& scene, GLFWwindow* window);
}
