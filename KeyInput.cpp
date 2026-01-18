#include "KeyInput.h"
#include <algorithm>
#include "Audio.h"

namespace KeyInput
{
    void applyKeyMapping(AppContext& ctx, bool pianoMode)
    {
        int (*km)[10] = pianoMode ? ctx.keyMapPiano : ctx.keyMapBtn;
        if (!km) return;

        std::vector<Audio::ButtonSound> audioButtons;
        audioButtons.reserve(ctx.baseN);

        for (int r = 0; r < ctx.baseRows; r++) {
            for (int c = 0; c < ctx.cols; c++) {
                int i = r * ctx.cols + c;

                Audio::ButtonSound bs;
                bs.key = km[r][c];
                bs.filePath = ctx.buttonFiles[i];
                bs.isPressed = false;
                bs.sound = nullptr;

                audioButtons.push_back(bs);
            }
        }

        Audio::setButtons(audioButtons);

        ctx.keyToButtons.assign(GLFW_KEY_LAST + 1, {});
        for (int i = 0; i < ctx.baseN; i++) {
            int k = audioButtons[i].key;
            if (k >= 0 && k <= GLFW_KEY_LAST) {
                ctx.keyToButtons[k].push_back(i);            // original
                ctx.keyToButtons[k].push_back(i + ctx.baseN); // klon
            }
        }
    }

    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        (void)scancode; (void)mods;

        AppContext* ctx = static_cast<AppContext*>(glfwGetWindowUserPointer(window));
        if (!ctx) return;

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
            return;
        }

        if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
            ctx->pianoMode = !ctx->pianoMode;

            std::fill(ctx->pressTarget.begin(), ctx->pressTarget.end(), 0.0f);
            applyKeyMapping(*ctx, ctx->pianoMode);

            return;
        }

        Audio::handleKey(key, action);

        if (key >= 0 && key < (int)ctx->keyToButtons.size()) {
            const auto& btnList = ctx->keyToButtons[key];
            for (int btnIdx : btnList) {
                if (btnIdx >= 0 && btnIdx < (int)ctx->pressTarget.size()) {
                    if (action == GLFW_PRESS)   ctx->pressTarget[btnIdx] = 1.0f;
                    if (action == GLFW_RELEASE) ctx->pressTarget[btnIdx] = 0.0f;
                }
            }
        }
    }
}
