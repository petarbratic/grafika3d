// Autori: Nedeljko Tesanovic i Vasilije Markovic
// Opis: Harmonika dugmetara – demo sa teksturama nota + Audio (irrKlang)

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Util.h"
#include "Audio.h"

#include "AppContext.h"
#include "Common.h"
#include "NoteData.h"
#include "TextureManager.h"
#include "KeyInput.h"
#include "AccordionScene.h"

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

    if (glewInit() != GLEW_OK) {
        std::cout << "GLEW greska\n";
        return 3;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // ------------------------------------------------------------
    // AppContext (shared state)
    AppContext ctx;

    // ------------------------------------------------------------
    // Audio init (popravljen bug: init samo jednom)
    ctx.audioOk = Audio::init();
    if (!ctx.audioOk) {
        std::cout << "Audio init nije uspeo (irrKlang)\n";
    }

    // ------------------------------------------------------------
    // Shader
    unsigned int shader = createShader("basic.vert", "basic.frag");
    glUseProgram(shader);

    int modelLoc = glGetUniformLocation(shader, "uM");
    int viewLoc = glGetUniformLocation(shader, "uV");
    int projLoc = glGetUniformLocation(shader, "uP");
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

    glActiveTexture(GL_TEXTURE0);

    // ------------------------------------------------------------
    // Teksture
    TextureManager textures;
    textures.loadAll();

    // ------------------------------------------------------------
    // Scena (mesh + pozicije)
    AccordionScene scene;
    scene.init(&textures);

    // ------------------------------------------------------------
    // Key mapping (tvoje mape)
    static int keyMap[3][10] = {
        { GLFW_KEY_Q, GLFW_KEY_W, GLFW_KEY_E, GLFW_KEY_R, GLFW_KEY_T, GLFW_KEY_Y, GLFW_KEY_U, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
        { GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_J, GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_SEMICOLON },
        { GLFW_KEY_TAB, GLFW_KEY_Z, GLFW_KEY_X, GLFW_KEY_C, GLFW_KEY_V, GLFW_KEY_B, GLFW_KEY_N, GLFW_KEY_M, GLFW_KEY_COMMA, GLFW_KEY_PERIOD }
    };

    static int keyMap2[3][10] = {
        { GLFW_KEY_A, GLFW_KEY_X, GLFW_KEY_V, GLFW_KEY_H, GLFW_KEY_K, GLFW_KEY_PERIOD, GLFW_KEY_9, GLFW_KEY_I, GLFW_KEY_O, GLFW_KEY_P },
        { GLFW_KEY_Z, GLFW_KEY_D, GLFW_KEY_G, GLFW_KEY_N, GLFW_KEY_COMMA, GLFW_KEY_SEMICOLON, GLFW_KEY_9, GLFW_KEY_SLASH, GLFW_KEY_9, GLFW_KEY_9 },
        { GLFW_KEY_TAB, GLFW_KEY_S, GLFW_KEY_C, GLFW_KEY_B, GLFW_KEY_M, GLFW_KEY_L, GLFW_KEY_9, GLFW_KEY_9, GLFW_KEY_9, GLFW_KEY_PERIOD }
    };

    ctx.baseRows = scene.baseRows();
    ctx.cols = scene.cols();
    ctx.baseN = scene.baseButtons();

    ctx.keyMapBtn = keyMap;
    ctx.keyMapPiano = keyMap2;

    // button files (0..29) iz layout + wav
    ctx.buttonFiles = buildButtonFilesFromLayout();

    // pressTarget za svih 60 dugmadi (3x10 + klon 3x10)
    ctx.pressTarget.assign(scene.totalButtons(), 0.0f);

    // apply mapping (inicijalno dugmetarski)
    KeyInput::applyKeyMapping(ctx, false);

    // callback: ctx preko window user pointer-a
    glfwSetWindowUserPointer(window, &ctx);
    glfwSetKeyCallback(window, KeyInput::keyCallback);

    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

    double lastTime = glfwGetTime();

    // ------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;
        dt = clampf(dt, 0.0f, 0.05f);

        scene.updateRotationFromKeys(window);
        scene.updatePress(dt, ctx.pressTarget);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        scene.render(shader, modelLoc);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ------------------------------------------------------------
    Audio::shutdown();

    glDeleteProgram(shader);
    scene.shutdown();

    glfwTerminate();
    return 0;
}
