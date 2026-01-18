// Audio.h - 3D harmonika dugmetara (irrKlang + GLFW key handling)

#pragma once

#include <vector>
#include <irrKlang.h>

namespace Audio {

    struct ButtonSound {
        int key = 0;                    // GLFW_KEY_...
        const char* filePath = nullptr; // putanja do .wav/.ogg
        bool isPressed = false;         // stanje (drzi li se taster)
        irrklang::ISound* sound = nullptr; // irrKlang handle (interno upravljanje)
    };

    // Init/gasenje irrKlang-a
    bool init();
    void shutdown();

    // Postavi raspored dugmadi (pozovi jednom posle kreiranja layout-a)
    void setButtons(const std::vector<ButtonSound>& buttons);

    // Pozivaj iz GLFW key callback-a: prosledi key i action
    void handleKey(int key, int action);

    // Meh aktivan ako je bar jedno dugme pritisnuto
    bool isBellowsActive();

    // Koliko dugmadi je trenutno pritisnuto
    int pressedCount();

} // namespace Audio
