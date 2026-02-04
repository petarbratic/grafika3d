#include "AppSetup.h"

#include "AppContext.h"
#include "AccordionScene.h"
#include "KeyInput.h"
#include "Audio.h"
#include "Util.h"

namespace AppSetup
{
    bool initAudio(AppContext& ctx)
    {
        ctx.audioOk = Audio::init();
        return ctx.audioOk;
    }

    void shutdownAudio()
    {
        Audio::shutdown();
    }

    void setupContextAndScene(AppContext& ctx, AccordionScene& scene, GLFWwindow* window)
    {
        // Scena (bez tekstura nota, kao kod tebe)
        scene.init(nullptr);

        // Key mapping (tvoje mape)
        static int keyMap[3][10] = {
            { GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
            { GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_SEMICOLON },
            { GLFW_KEY_TAB, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_B, GLFW_KEY_N, GLFW_KEY_M, GLFW_KEY_COMMA, GLFW_KEY_PERIOD }
        };

        static int keyMap2[3][10] = {
            { GLFW_KEY_A, GLFW_KEY_X, GLFW_KEY_V, GLFW_KEY_H, GLFW_KEY_K, GLFW_KEY_PERIOD, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE },
            { GLFW_KEY_Z, GLFW_KEY_D, GLFW_KEY_G, GLFW_KEY_N, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_BACKSPACE, GLFW_KEY_SLASH, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE },
            { GLFW_KEY_TAB, GLFW_KEY_S, GLFW_KEY_C, GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_L, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_BACKSPACE, GLFW_KEY_PERIOD }
        };

        // DUGMETARSKI: osnovni + dur
        ctx.bassKeyBtn_basic[0] = GLFW_KEY_8;
        ctx.bassKeyBtn_basic[1] = GLFW_KEY_9;
        ctx.bassKeyBtn_basic[2] = GLFW_KEY_7;
        ctx.bassKeyBtn_basic[3] = GLFW_KEY_6;
        ctx.bassKeyBtn_basic[4] = GLFW_KEY_5;
        ctx.bassKeyBtn_basic[5] = GLFW_KEY_4;
        ctx.bassKeyBtn_basic[6] = GLFW_KEY_3;
        ctx.bassKeyBtn_basic[7] = GLFW_KEY_2;

        ctx.bassKeyBtn_dur[0] = GLFW_KEY_F8;
        ctx.bassKeyBtn_dur[1] = GLFW_KEY_F9;
        ctx.bassKeyBtn_dur[2] = GLFW_KEY_F7;
        ctx.bassKeyBtn_dur[3] = GLFW_KEY_F6;
        ctx.bassKeyBtn_dur[4] = GLFW_KEY_F5;
        ctx.bassKeyBtn_dur[5] = GLFW_KEY_F4;
        ctx.bassKeyBtn_dur[6] = GLFW_KEY_F3;
        ctx.bassKeyBtn_dur[7] = GLFW_KEY_F2;

        // KLAVIRNI: osnovni + dur + mol
        ctx.bassKeyPiano_basic[0] = GLFW_KEY_I;
        ctx.bassKeyPiano_basic[1] = GLFW_KEY_O;
        ctx.bassKeyPiano_basic[2] = GLFW_KEY_U;
        ctx.bassKeyPiano_basic[3] = GLFW_KEY_Y;
        ctx.bassKeyPiano_basic[4] = GLFW_KEY_T;
        ctx.bassKeyPiano_basic[5] = GLFW_KEY_R;
        ctx.bassKeyPiano_basic[6] = GLFW_KEY_E;
        ctx.bassKeyPiano_basic[7] = GLFW_KEY_W;

        ctx.bassKeyPiano_dur[0] = GLFW_KEY_8;
        ctx.bassKeyPiano_dur[1] = GLFW_KEY_9;
        ctx.bassKeyPiano_dur[2] = GLFW_KEY_7;
        ctx.bassKeyPiano_dur[3] = GLFW_KEY_6;
        ctx.bassKeyPiano_dur[4] = GLFW_KEY_5;
        ctx.bassKeyPiano_dur[5] = GLFW_KEY_4;
        ctx.bassKeyPiano_dur[6] = GLFW_KEY_3;
        ctx.bassKeyPiano_dur[7] = GLFW_KEY_2;

        ctx.bassKeyPiano_mol[0] = GLFW_KEY_F8;
        ctx.bassKeyPiano_mol[1] = GLFW_KEY_F9;
        ctx.bassKeyPiano_mol[2] = GLFW_KEY_F7;
        ctx.bassKeyPiano_mol[3] = GLFW_KEY_F6;
        ctx.bassKeyPiano_mol[4] = GLFW_KEY_F5;
        ctx.bassKeyPiano_mol[5] = GLFW_KEY_F4;
        ctx.bassKeyPiano_mol[6] = GLFW_KEY_F3;
        ctx.bassKeyPiano_mol[7] = GLFW_KEY_F2;

        ctx.baseRows = scene.baseRows();
        ctx.cols = scene.cols();
        ctx.baseN = scene.baseButtons();

        ctx.keyMapBtn = keyMap;
        ctx.keyMapPiano = keyMap2;

        // Audio fajlovi
        ctx.buttonFiles = buildButtonFilesFromLayout();
        ctx.bassFiles = buildBassFiles();

        ctx.pressTarget.assign(scene.totalButtons(), 0.0f);

        // Pocetno mapiranje
        KeyInput::applyKeyMapping(ctx, false);

        // GLFW callbacks
        glfwSetWindowUserPointer(window, &ctx);
        glfwSetKeyCallback(window, KeyInput::keyCallback);
    }
}
