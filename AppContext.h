#pragma once
#include <vector>
#include <GLFW/glfw3.h>

// Zajednicko stanje programa (umesto gomile globalnih promenljivih)
struct AppContext
{
    // --- input / audio mapping ---
    std::vector<std::vector<int>> keyToButtons;  // GLFW key -> list indices
    std::vector<float> pressTarget;              // 0/1 cilj animacije
    bool pianoMode = false;
    bool audioOk = false;

    // --- mapping support (desna ruka) ---
    int baseRows = 3;
    int cols = 10;
    int baseN = 30; // 3*10

    // cuva fajl putanju za svako od prvih 30 dugmica (desna ruka)
    std::vector<const char*> buttonFiles;

    // pokazivaci na 3x10 key mape (desna ruka)
    int (*keyMapBtn)[10] = nullptr;
    int (*keyMapPiano)[10] = nullptr;

    // --- bass (leva ruka) ---
    // 8*3 = 24 fajla (osnovni, dur, mol) redom f b c g d a e h
    std::vector<const char*> bassFiles; // size 24

    // key mape za basove:
    // dugmetarski: osnovni(8) + dur(8) ; mol se ne koristi
    int bassKeyBtn_basic[8]{};
    int bassKeyBtn_dur[8]{};

    // klavirni: osnovni(8) + dur(8) + mol(8)
    int bassKeyPiano_basic[8]{};
    int bassKeyPiano_dur[8]{};
    int bassKeyPiano_mol[8]{};
};
