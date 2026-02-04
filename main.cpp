// Autori: Nedeljko Tesanovic i Vasilije Markovic
// Opis: Harmonika dugmetara – demo + Audio (irrKlang) + Phong osvetljenje + overlay (ime)
#include "GLCommon.h"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "Common.h"
#include "AccordionScene.h"
#include "AppContext.h"

#include "GraphicsApp.h"
#include "AppSetup.h"
#include "PhongPipeline.h"
#include "OverlayRenderer.h"
#include "InputToggles.h"

int main()
{
    GraphicsApp app;
    if (!app.initFullscreen("Harmonika dugmetara")) {
        return 1;
    }

    app.setClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    AppContext ctx;

    ctx.audioOk = AppSetup::initAudio(ctx);
    if (!ctx.audioOk) {
        std::cout << "Audio init nije uspeo (irrKlang)\n";
    }

    AccordionScene scene;
    AppSetup::setupContextAndScene(ctx, scene, app.window());

    InputToggles toggles;

    PhongPipeline phong;
    if (!phong.init(app.width(), app.height())) {
        std::cout << "Phong shader init nije uspeo\n";
    }

    OverlayRenderer overlay;
    overlay.init("media/ime.png");

    // Materijali (isto kao ranije)
    glm::vec3 body_kA(0.02f);
    glm::vec3 body_kD(0.05f);
    glm::vec3 body_kS(0.15f);
    float body_shine = 64.0f;

    glm::vec3 btn_kA(0.10f);
    glm::vec3 btn_kD(0.75f);
    glm::vec3 btn_kS(0.30f);
    float btn_shine = 96.0f;

    double lastTime = app.timeNow();

    while (!app.shouldClose())
    {
        toggles.update(app.window());

        const double frameStart = app.timeNow();
        float dt = (float)(frameStart - lastTime);
        lastTime = frameStart;
        dt = clampf(dt, 0.0f, 0.05f);

        scene.updateRotationFromKeys(app.window());
        scene.updatePress(dt, ctx.pressTarget);

        app.beginFrame();

        phong.use();
        scene.renderPhong(phong.program(), phong.modelLoc(),
            body_kA, body_kD, body_kS, body_shine,
            btn_kA, btn_kD, btn_kS, btn_shine);
        phong.stop();

        overlay.draw();

        app.endFrame();
        app.limitFps(75.0, frameStart);
    }

    // Shutdown redosled
    AppSetup::shutdownAudio();

    overlay.shutdown();
    phong.shutdown();
    scene.shutdown();

    app.shutdown();
    return 0;
}
