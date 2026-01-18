#pragma once
#include <vector>
#include <GLFW/glfw3.h>

// Zajednicko stanje programa (umesto gomile globalnih promenljivih)
struct AppContext
{
    // --- input / audio mapping ---
    std::vector<std::vector<int>> keyToButtons;  // GLFW key -> list indices (original + clone)
    std::vector<float> pressTarget;              // 0/1 cilj animacije
    bool pianoMode = false;
    bool audioOk = false;

    // --- mapping support ---
    int baseRows = 3;
    int cols = 10;
    int baseN = 30; // 3*10

    // cuva fajl putanju za svako od prvih 30 dugmica
    std::vector<const char*> buttonFiles;

    // pokazivaci na 3x10 key mape
    int (*keyMapBtn)[10] = nullptr;
    int (*keyMapPiano)[10] = nullptr;
};
