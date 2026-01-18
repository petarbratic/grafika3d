// Autori: Nedeljko Tesanovic i Vasilije Markovic
// Opis: Harmonika dugmetara – demo sa teksturama nota + Audio (irrKlang)

#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"
#include "ButtonCylinder.h"
#include "Audio.h"

// ------------------------------------------------------------
// Pomocne funkcije
static float clampf(float v, float lo, float hi) {
    return std::max(lo, std::min(hi, v));
}

// OBAVEZNO: tekstura sa filterima da ne bude crna
static unsigned int preprocessTexture(const char* filepath) {
    unsigned int tex = loadImageToTexture(filepath);
    if (tex == 0) {
        std::cout << "Neuspesno ucitana textura: " << filepath << std::endl;
        return 0;
    }

    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    return tex;
}
// ------------------------------------------------------------

// Global za input callback
static std::vector<std::vector<int>> g_keyToButtons;          
static std::vector<float> g_pressTarget;        // 0/1 cilj animacije
static std::vector<unsigned int> g_buttonSound; // map: index dugmeta -> index zvuka u Audio::buttons (ovde 1:1)

static bool g_pianoMode = false;   // false=dugmetarski, true=klavirski
static bool g_audioOk = false;


// --- Toggle mapping support (0 = dugmetarski, 1 = klavirski) ---
static int g_baseRows = 3;
static int g_cols = 10;
static int g_baseN = 30;

// cuva fajl putanju za svako od prvih 30 dugmica (ne menja se, samo se menjaju tasteri)
static std::vector<const char*> g_buttonFiles;

// pokazivaci na 3x10 key mape (keyMap i keyMap2)
static int (*g_keyMapBtn)[10] = nullptr;
static int (*g_keyMapPiano)[10] = nullptr;

static void applyKeyMapping(bool pianoMode)
{
    // izaberi aktivnu mapu
    int (*km)[10] = pianoMode ? g_keyMapPiano : g_keyMapBtn;
    if (!km) return; // jos nije postavljeno u main-u

    // napravi Audio dugmice sa NOVIM key-evima, ali ISTIM filePath-ovima
    std::vector<Audio::ButtonSound> audioButtons;
    audioButtons.reserve(g_baseN);

    for (int r = 0; r < g_baseRows; r++) {
        for (int c = 0; c < g_cols; c++) {
            int i = r * g_cols + c;

            Audio::ButtonSound bs;
            bs.key = km[r][c];
            bs.filePath = g_buttonFiles[i];
            bs.isPressed = false;
            bs.sound = nullptr;

            audioButtons.push_back(bs);
        }
    }

    Audio::setButtons(audioButtons);

    // rebuild: GLFW_KEY_* -> list dugmica (original + klon)
    g_keyToButtons.assign(GLFW_KEY_LAST + 1, {});
    for (int i = 0; i < g_baseN; i++) {
        int k = audioButtons[i].key;
        if (k >= 0 && k <= GLFW_KEY_LAST) {
            g_keyToButtons[k].push_back(i);          // original
            g_keyToButtons[k].push_back(i + g_baseN); // klon
        }
    }
}




static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
        return;
    }


    if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
        g_pianoMode = !g_pianoMode;

        // opcionalno: ugasi animacije pritiska da nista ne ostane "zaglavljeno"
        std::fill(g_pressTarget.begin(), g_pressTarget.end(), 0.0f);

        // PROMENA MAPIRANJA (dugmetarski <-> klavirski)
        applyKeyMapping(g_pianoMode);

        return; // bitno: da ne prosledis ovaj key Audio sistemu kao "nota"
    }

    // prosledi Audio sistemu (on sam pali/gasi zvukove koje si registrovao)
    Audio::handleKey(key, action);

    // ako je taster mapiran na neko 3D dugme, ukljuci/iskljuci utiskivanje
    if (key >= 0 && key < (int)g_keyToButtons.size()) {
        const auto& btnList = g_keyToButtons[key];
        for (int btnIdx : btnList) {
            if (btnIdx >= 0 && btnIdx < (int)g_pressTarget.size()) {
                if (action == GLFW_PRESS)   g_pressTarget[btnIdx] = 1.0f;
                if (action == GLFW_RELEASE) g_pressTarget[btnIdx] = 0.0f;
            }
        }
    }
}


int main(void)
{
    if (!glfwInit()) {
        std::cout << "GLFW nije ucitan\n";
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Harmonika dugmetara", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 2;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW greska\n";
        return 3;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ------------------------------------------------------------
    // Audio init
    if (!Audio::init()) {

        std::cout << "Audio init nije uspeo (irrKlang)\n";
        // ne prekidamo program - samo nema zvuka
    }
    g_audioOk = Audio::init();
    if (!g_audioOk) std::cout << "Audio init nije uspeo (irrKlang)\n";

    // ------------------------------------------------------------
    // Shader
    unsigned int shader = createShader("basic.vert", "basic.frag");
    glUseProgram(shader);

    unsigned int modelLoc = glGetUniformLocation(shader, "uM");
    unsigned int viewLoc = glGetUniformLocation(shader, "uV");
    unsigned int projLoc = glGetUniformLocation(shader, "uP");
    glUniform1i(glGetUniformLocation(shader, "uTex"), 0);

    // ------------------------------------------------------------
    // Kamera
    glm::mat4 view = glm::lookAt(
        glm::vec3(0, 0, 2.2f),
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );

    glm::mat4 proj = glm::perspective(
        glm::radians(70.0f),
        1.0f,
        0.1f,
        100.0f
    );

    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

    // ------------------------------------------------------------
    // Teksture nota
    unsigned int texC = preprocessTexture("notes/C.png");
    unsigned int texCsharp = preprocessTexture("notes/c#.png");
    unsigned int texD = preprocessTexture("notes/d.png");
    unsigned int texDsharp = preprocessTexture("notes/d#.png");
    unsigned int texE = preprocessTexture("notes/e.png");
    unsigned int texF = preprocessTexture("notes/f.png");
    unsigned int texFsharp = preprocessTexture("notes/f#.png");
    unsigned int texG = preprocessTexture("notes/g.png");
    unsigned int texGsharp = preprocessTexture("notes/g#.png");
    unsigned int texA = preprocessTexture("notes/a.png");
    unsigned int texBflat = preprocessTexture("notes/Bb.png");
    unsigned int texB = preprocessTexture("notes/h.png");

    enum NoteId {
        F0, FSHARP0, G0, GSHARP0, A0, BFLAT0, B0,  C, CSHARP, D, DSHARP, E, F, FSHARP, G, GSHARP, A, BB, B, C2, CSHARP2, D2, DSHARP2, E2, F2, FSHARP2, G2 ,GSHARP2, A2, NOTE_COUNT
    };

    unsigned int noteTex[NOTE_COUNT] = {};
    noteTex[F0] = texF;
    noteTex[FSHARP0] = texFsharp;
    noteTex[G0] = texG;
    noteTex[GSHARP0] = texGsharp;
    noteTex[A0] = texA;
    noteTex[BFLAT0] = texBflat;
    noteTex[B0] = texB;
    noteTex[C] = texC;
    noteTex[CSHARP] = texCsharp;
    noteTex[D] = texD;
    noteTex[DSHARP] = texDsharp;
    noteTex[E] = texE;
    noteTex[F] = texF;
    noteTex[FSHARP] = texFsharp;
    noteTex[G] = texG;
    noteTex[GSHARP] = texGsharp;
    noteTex[A] = texA;
    noteTex[BB] = texBflat;
    noteTex[B] = texB;
	noteTex[C2] = texC; 
    noteTex[CSHARP2] = texCsharp;
    noteTex[D2] = texD;
    noteTex[DSHARP2] = texDsharp;
    noteTex[E2] = texE;
    noteTex[F2] = texF;
    noteTex[FSHARP2] = texFsharp;
    noteTex[G2] = texG;
    noteTex[GSHARP2] = texGsharp;
    noteTex[A2] = texA;



    // Layout tekstura (tvoj raspored)
    NoteId layout[3][10] = {
        { FSHARP0, A0, C, DSHARP, FSHARP, A, C2, DSHARP2, FSHARP2, A2 },
        { G0, BFLAT0, CSHARP, E, G, BB, CSHARP2, E2, G2, BB },
        { F0, GSHARP0, B0, D, F, GSHARP, B, D2, F2, GSHARP2 }
    };

    glActiveTexture(GL_TEXTURE0);

    // ------------------------------------------------------------
    // Dugme (valjak)
    ButtonMesh btn = CreateButtonCylinder(
        0.06f,
        0.03f,
        32,
        { 0.9f, 0.9f, 0.9f, 1.0f }
    );

    // ------------------------------------------------------------
    // Raspored dugmadi
    const int baseRows = 3;
    const int cols = 10;
    const int totalRows = 6;
    const int N = totalRows * cols;
    const int baseN = baseRows * cols; // 30

    // postavi globale (za toggle mapiranja)
    g_baseRows = baseRows;
    g_cols = cols;
    g_baseN = baseN;

    std::vector<glm::vec3> localPos(N);
    std::vector<float> press(N, 0.0f);

    float dx = 0.16f;
    float dy = 0.14f;

    float startX = -0.5f * (cols - 1) * dx;
    float startY = 0.5f * (baseRows - 1) * dy;

    float rowOffset[3] = { 0.0f, 0.08f, 0.0f };

    // ------------------------------------------------------------
    // PODESAVANJE POZICIJE KLONA (samo menjaj ove vrednosti)

    // dodatni razmak izmedju 2 bloka (u "svetskim" jedinicama)
    float cloneGap = 0.25f;

    // osnovni pomeraj klona udesno (automatski racunat iz dx i broja kolona)
    float cloneBaseOffsetX = cols * dx + cloneGap;

    // fine-tuning klona (dodatno pomeranje)
    float cloneTuneX = -1.85f;   // levo(-) / desno(+)
    float cloneTuneY = +0.43f;   // dole(-) / gore(+)
    float cloneTuneZ = 0.00f;   // od kamere(-) / ka kameri(+)

    // finalni offset klona
    float cloneOffsetX = cloneBaseOffsetX + cloneTuneX;
    float cloneOffsetY = cloneTuneY;
    float cloneOffsetZ = cloneTuneZ;
	

    for (int r = 0; r < totalRows; r++) {
        int baseR = r % baseRows;               // 0..2

        float blockX = 0.0f;
        float blockY = 0.0f;
        float blockZ = 0.0f;

        // r >= 3 znaci da je klonirani blok
        if (r >= baseRows) {
            blockX = cloneOffsetX;
            blockY = cloneOffsetY;
            blockZ = cloneOffsetZ;
        }

        for (int c = 0; c < cols; c++) {
            int i = r * cols + c;
            localPos[i] = glm::vec3(
                (startX + c * dx + rowOffset[baseR]) + blockX,
                (startY - baseR * dy) + blockY,
                0.0f + blockZ
            );
        }
    }

    // ------------------------------------------------------------
    // Harmonika – globalna transformacija
    glm::vec3 accPos(0, 0, 0);
    float accRotX = 0.0f;
    float accRotY = 0.0f;

    float pressDepth = 0.025f;

    // ------------------------------------------------------------
    // KEY MAPPING (OVDE MENJAS KOJI TASTER SVIRA KOJE DUGME)
    //
    // Ideja:
    // - Imamo 30 dugmica (3x10).
    // - Dodeljujemo im 30 tastera.
    // - Kada pritisnes taster,:
    //     - Audio::handleKey ga pretvori u zvuk (po mapi u Audio::buttons)
    //     - mi ovde utisnemo odgovarajuce 3D dugme
    //
    // Najlaksa mapa za probu (menjaj kako hoces):
    // 1. red:  Q W E R T Y U I O P
    // 2. red:  A S D F G H J K L ;
    // 3. red:  Z X C V B N M , . /
    //
    // Napomena: GLFW za ; je GLFW_KEY_SEMICOLON, za , je GLFW_KEY_COMMA itd.
    int keyMap[3][10] = {
        { GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
        { GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_SEMICOLON },
        { GLFW_KEY_TAB, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_B, GLFW_KEY_N, GLFW_KEY_M, GLFW_KEY_COMMA, GLFW_KEY_PERIOD }
    };

    int keyMap2[3][10] = {
        { GLFW_KEY_A, GLFW_KEY_X, GLFW_KEY_V, GLFW_KEY_H, GLFW_KEY_K, GLFW_KEY_PERIOD, GLFW_KEY_9, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
        { GLFW_KEY_Z, GLFW_KEY_D, GLFW_KEY_G, GLFW_KEY_N, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_9, GLFW_KEY_SLASH, GLFW_KEY_9, GLFW_KEY_9 },
        { GLFW_KEY_TAB, GLFW_KEY_S, GLFW_KEY_C, GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_L, GLFW_KEY_9, GLFW_KEY_9, GLFW_KEY_9, GLFW_KEY_PERIOD }
    };


    g_keyMapBtn = keyMap;
    g_keyMapPiano = keyMap2;


    // ------------------------------------------------------------
    // AUDIO MAPPING (OVDE MENJAS KOJI ZVUK IDE NA KOJE DUGME)
    //
    // Ti mozes da odlucis da li zelis:
    // - 30 razlicitih fajlova (jedan po dugmetu)
    // - ili samo 12 fajlova (C, C#, D, ...) pa da se ponavljaju
    //
    // Za sada: pravimo 12 zvukova (po NoteId), a dugme dobije zvuk po layout[r][c].
    //
    // IMPORTANT: putanje prilagodi tvojim fajlovima.
    // Primer: "audio/C.wav", "audio/c#.wav", itd.
    //
    // Ako hoces kasnije oktave, onda ces imati npr. "audio/C4.wav", "audio/C5.wav" itd.
    const char* noteWav[NOTE_COUNT] = {
        "semplHarmonika/f0.wav",
        "semplHarmonika/f#0.wav",
        "semplHarmonika/g0.wav",
        "semplHarmonika/g#0.wav",
        "semplHarmonika/a0.wav",
        "semplHarmonika/b0.wav",
        "semplHarmonika/h0.wav",
        "semplHarmonika/C.wav",
        "semplHarmonika/c#.wav",
        "semplHarmonika/d.wav",
        "semplHarmonika/d#.wav",
        "semplHarmonika/e.wav",
        "semplHarmonika/f.wav",
        "semplHarmonika/f#.wav",
        "semplHarmonika/g.wav",
        "semplHarmonika/g#.wav",
        "semplHarmonika/a.wav",
        "semplHarmonika/Bb.wav",
        "semplHarmonika/h.wav",
         "semplHarmonika/c2.wav",
         "semplHarmonika/c#2.wav",
        "semplHarmonika/d2.wav",
        "semplHarmonika/d#2.wav",
        "semplHarmonika/e2.wav",
        "semplHarmonika/f2.wav",
        "semplHarmonika/f#2.wav",
         "semplHarmonika/g2.wav",
         "semplHarmonika/g#2.wav",
         "semplHarmonika/a2.wav"



    };

    // Napravi listu dugmadi za Audio sistem: svako dugme ima svoj taster i fajl.
   // zapamti fajl putanje po dugmetu (0..29) - ovo ostaje isto u oba rezima
    g_buttonFiles.assign(baseN, nullptr);
    for (int r = 0; r < baseRows; r++) {
        for (int c = 0; c < cols; c++) {
            int i = r * cols + c;
            g_buttonFiles[i] = noteWav[layout[r][c]];
        }
    }

    // inicijalno: dugmetarski raspored (keyMap)
    applyKeyMapping(false);

    // Cilj animacije pritiska (0/1)
    g_pressTarget.assign(N, 0.0f);

    // ------------------------------------------------------------
    // Animacija pritiska (glatko)
    float pressInSpeed = 10.0f;
    float pressOutSpeed = 14.0f;

    static double lastTime = glfwGetTime();

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    // ------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // dt
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;
        dt = clampf(dt, 0.0f, 0.05f);

        // Rotacija cele harmonike (strelice) - ostaje
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) accRotY += 0.03f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) accRotY -= 0.03f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) accRotX += 0.03f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) accRotX -= 0.03f;

        /*if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) klavirna = 1;
		if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) klavirna = 0;*/

        // Glatko prati target iz keyCallback-a
        for (int i = 0; i < N; i++) {
            float target = g_pressTarget[i];
            float speed = (target > press[i]) ? pressInSpeed : pressOutSpeed;
            press[i] = press[i] + (target - press[i]) * (1.0f - std::exp(-speed * dt));
            press[i] = clampf(press[i], 0.0f, 1.0f);
        }

        // ----- CRTANJE -----
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        glm::mat4 accM(1.0f);
        accM = glm::translate(accM, accPos);
        accM = glm::rotate(accM, glm::radians(-90.0f), glm::vec3(0, 0, 1));  // pocetna orijentacija
        accM = glm::rotate(accM, accRotY, glm::vec3(0, 1, 0));
        accM = glm::rotate(accM, accRotX, glm::vec3(1, 0, 0));

        glBindVertexArray(btn.VAO);

        for (int i = 0; i < N; i++)
        {
            glm::mat4 local(1.0f);
            local = glm::translate(local, localPos[i]);

            glm::mat4 orient(1.0f);
            orient = glm::rotate(orient, glm::radians(-90.0f), glm::vec3(1, 0, 0)); // dugme "legne"

            glm::mat4 pressM(1.0f);
            // utiskivanje po +Y u lokalnom prostoru (kod tebe je ovako radilo); promeni znak ako hoces na dole
            pressM = glm::translate(pressM, glm::vec3(0, pressDepth * press[i], 0));

            glm::mat4 M = accM * local * orient * pressM;
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(M));

            int r = i / cols;
            int c = i % cols;

            int baseR = r % baseRows;       // 0..2
            NoteId n = layout[baseR][c];

            
            glBindTexture(GL_TEXTURE_2D, noteTex[n]);

            glUniform1i(glGetUniformLocation(shader, "useTex"), 1);
            glUniform1i(glGetUniformLocation(shader, "transparent"), 1);

            glDrawArrays(GL_TRIANGLES, 0, btn.vertexCount);
        }

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ------------------------------------------------------------
    Audio::shutdown();

    glDeleteProgram(shader);
    DestroyButtonMesh(btn);
    glfwTerminate();
    return 0;
}
