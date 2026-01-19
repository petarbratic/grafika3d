#include "KeyInput.h"
#include <algorithm>
#include "Audio.h"

// Napomena: indeksiranje dugmadi u pressTarget:
// [0 .. baseN-1]        desna original (30)
// [baseN .. 2*baseN-1]  desna klon (30)
// [2*baseN .. 2*baseN + 24 - 1] bass (24)

namespace KeyInput
{
    static inline void addKeyToButtons(AppContext& ctx, int key, int btnIdx)
    {
        if (key >= 0 && key <= GLFW_KEY_LAST)
            ctx.keyToButtons[key].push_back(btnIdx);
    }

    void applyKeyMapping(AppContext& ctx, bool pianoMode)
    {
        int (*km)[10] = pianoMode ? ctx.keyMapPiano : ctx.keyMapBtn;
        if (!km) return;

        std::vector<Audio::ButtonSound> audioButtons;

        // desna ruka: 30
        audioButtons.reserve(ctx.baseN + 24);

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

        // bass: dodaj u Audio listu (da Audio::handleKey zna za njih)
        // ključevi zavise od moda
        if (!ctx.bassFiles.empty())
        {
            if (!pianoMode)
            {
                // dugmetarski: osnovni(0..7) + dur(8..15)
                for (int i = 0; i < 8; i++)
                {
                    Audio::ButtonSound bs{};
                    bs.key = ctx.bassKeyBtn_basic[i];
                    bs.filePath = ctx.bassFiles[i]; // BAS_F..BAS_H
                    audioButtons.push_back(bs);
                }
                for (int i = 0; i < 8; i++)
                {
                    Audio::ButtonSound bs{};
                    bs.key = ctx.bassKeyBtn_dur[i];
                    bs.filePath = ctx.bassFiles[8 + i]; // BAS_*_DUR
                    audioButtons.push_back(bs);
                }
            }
            else
            {
                // klavirni: osnovni + dur + mol
                for (int i = 0; i < 8; i++)
                {
                    Audio::ButtonSound bs{};
                    bs.key = ctx.bassKeyPiano_basic[i];
                    bs.filePath = ctx.bassFiles[i];
                    audioButtons.push_back(bs);
                }
                for (int i = 0; i < 8; i++)
                {
                    Audio::ButtonSound bs{};
                    bs.key = ctx.bassKeyPiano_dur[i];
                    bs.filePath = ctx.bassFiles[8 + i];
                    audioButtons.push_back(bs);
                }
                for (int i = 0; i < 8; i++)
                {
                    Audio::ButtonSound bs{};
                    bs.key = ctx.bassKeyPiano_mol[i];
                    bs.filePath = ctx.bassFiles[16 + i];
                    audioButtons.push_back(bs);
                }
            }
        }

        Audio::setButtons(audioButtons);

        // mapiranje key -> pressTarget indeksi
        ctx.keyToButtons.assign(GLFW_KEY_LAST + 1, {});

        // desna ruka: mapiraj original + klon
        for (int i = 0; i < ctx.baseN; i++) {
            int k = audioButtons[i].key;
            if (k >= 0 && k <= GLFW_KEY_LAST) {
                addKeyToButtons(ctx, k, i);              // original
                addKeyToButtons(ctx, k, i + ctx.baseN);  // klon
            }
        }

        // bass dugmad: pressTarget indeksi počinju od bassBaseIndex
        const int bassBaseIndex = ctx.baseN * 2;

        if (!ctx.bassFiles.empty())
        {
            if (!pianoMode)
            {
                // u audioButtons su dodata 16 bass dugmeta, odmah posle 30 desnih
                // njihovi audioButtons indeksi:
                // 30..37 basic, 38..45 dur
                int audioStart = ctx.baseN;

                for (int i = 0; i < 16; i++)
                {
                    int k = audioButtons[audioStart + i].key;
                    if (k >= 0 && k <= GLFW_KEY_LAST)
                        addKeyToButtons(ctx, k, bassBaseIndex + i);
                }
            }
            else
            {
                // u audioButtons su dodata 24 bass dugmeta posle 30 desnih
                int audioStart = ctx.baseN;
                for (int i = 0; i < 24; i++)
                {
                    int k = audioButtons[audioStart + i].key;
                    if (k >= 0 && k <= GLFW_KEY_LAST)
                        addKeyToButtons(ctx, k, bassBaseIndex + i);
                }
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
