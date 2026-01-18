// Audio.cpp - 3D harmonika dugmetara (irrKlang + GLFW key handling)
//
// Napomena:
// - Ovaj fajl ne zavisi od Graphics modula.
// - Radi sa jednim vektorom dugmadi (i za "bas" i za "desnu").
// - Na GLFW_PRESS startuje loop zvuka, na GLFW_RELEASE zaustavlja.
// - Odrzava brojac pritisnutih dugmadi (bellows active).
//
// Potrebno:
// - irrKlang (include path + link)
// - GLFW (zbog GLFW_PRESS/GLFW_RELEASE konstanti)

#include "Audio.h"

#include <GLFW/glfw3.h>
#include <irrKlang.h>

#include <iostream>
#include <vector>
#include <algorithm>

namespace Audio {

    using namespace irrklang;

    static ISoundEngine* g_engine = nullptr;

    // Sva dugmad (i bas i desna) u jednom nizu
    static std::vector<ButtonSound> g_buttons;

    // Koliko je trenutno pritisnuto dugmadi (meh "aktivan" ako je > 0)
    static int g_pressedCount = 0;

    // ------------------------------------------------------------
    // Pomocne funkcije
    static ButtonSound* findButtonByKey(int key) {
        for (auto& b : g_buttons) {
            if (b.key == key) return &b;
        }
        return nullptr;
    }

    static void startLoop(ButtonSound& b) {
        if (!g_engine) return;
        if (b.sound) return; // vec svira

        // play2D(..., looped, startPaused, track)
        b.sound = g_engine->play2D(
            b.filePath,
            true,   // looped
            false,  // startPaused
            true    // track -> dobijamo ISound*
        );

        if (!b.sound) {
            std::cout << "Audio: Ne mogu da pustim zvuk: " << (b.filePath ? b.filePath : "(null)") << "\n";
        }
    }

    static void stopAndDrop(ButtonSound& b) {
        if (!b.sound) return;

        b.sound->stop();
        b.sound->drop();
        b.sound = nullptr;
    }

    // ------------------------------------------------------------
    // Public API

    bool init() {
        if (g_engine) return true;

        g_engine = createIrrKlangDevice();
        if (!g_engine) {
            std::cout << "Audio: Greska - irrKlang engine nije mogao da se kreira.\n";
            return false;
        }
        return true;
    }

    void shutdown() {
        // Zaustavi sve zvuke pre gasenja
        for (auto& b : g_buttons) {
            b.isPressed = false;
            stopAndDrop(b);
        }
        g_pressedCount = 0;

        if (g_engine) {
            g_engine->drop();
            g_engine = nullptr;
        }
    }

    void setButtons(const std::vector<ButtonSound>& buttons) {
        // Oslobodi stare (ako postoje)
        for (auto& b : g_buttons) {
            b.isPressed = false;
            stopAndDrop(b);
        }
        g_pressedCount = 0;

        // Kopiraj novi raspored
        g_buttons = buttons;

        // Osiguraj da su sound pointeri cisti (kopija bi mogla da prenese stare vrednosti)
        for (auto& b : g_buttons) {
            b.isPressed = false;
            b.sound = nullptr;
        }
    }

    bool isBellowsActive() {
        return g_pressedCount > 0;
    }

    int pressedCount() {
        return g_pressedCount;
    }

    void handleKey(int key, int action) {
        ButtonSound* b = findButtonByKey(key);
        if (!b) return;

        if (action == GLFW_PRESS) {
            if (!b->isPressed) {
                b->isPressed = true;
                g_pressedCount++;
                startLoop(*b);
            }
        }
        else if (action == GLFW_RELEASE) {
            if (b->isPressed) {
                b->isPressed = false;
                g_pressedCount = std::max(0, g_pressedCount - 1);
                stopAndDrop(*b);
            }
        }
        // GLFW_REPEAT ignorišemo (pošto loop vec svira)
    }

} // namespace Audio
